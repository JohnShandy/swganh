/*
 This file is part of SWGANH. For more information, visit http://swganh.com

 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "mysql_character_provider.h"

#include <boost/lexical_cast.hpp>

#ifdef WIN32
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#include "anh/crc.h"
#include "swganh/app/swganh_kernel.h"
#include "anh/database/database_manager.h"
#include "anh/service/service_directory.h"

#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/sqlstring.h>

#include "swganh/messages/delete_character_reply_message.h"
#include "swganh/character/character_data.h"
#include "swganh/messages/delete_character_message.h"
#include "swganh/messages/client_create_character.h"
#include "swganh/messages/client_create_character_success.h"
#include "swganh/messages/client_create_character_failed.h"
#include "swganh/messages/client_random_name_request.h"
#include "swganh/messages/client_random_name_response.h"

#include "swganh/object/player/player.h"


#include "anh/logger.h"

using namespace std;
using namespace anh;
using namespace anh::app;
using namespace anh::database;
using namespace plugins::mysql_character;
using namespace swganh::character;
using namespace swganh::messages;

#ifdef WIN32
using std::wregex;
using std::wsmatch;
using std::regex_match;
using std::regex;
using std::regex_search;
#else
using boost::wregex;
using boost::wsmatch;
using boost::regex_match;
using boost::regex;
using boost::regex_search;
#endif

MysqlCharacterProvider::MysqlCharacterProvider(KernelInterface* kernel)
    : CharacterProviderInterface()
    , kernel_(kernel) 
{
	auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");

	// Load each table of restricted names.
	auto statement = std::shared_ptr<sql::PreparedStatement>(
		conn->prepareStatement("SELECT * FROM `name_profane`;")
		);

	auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());
	while(result_set->next())
	{
		profane_names_.push_back(result_set->getString("name"));
	}

	statement.reset(conn->prepareStatement("SELECT * FROM `name_reserved`;"));
	result_set.reset(statement->executeQuery());
	while(result_set->next())
	{
		reserved_names_.push_back(result_set->getString("name"));
	}

	statement.reset(conn->prepareStatement("SELECT * FROM `name_fictionally_reserved`;"));
	result_set.reset(statement->executeQuery());
	while(result_set->next())
	{
		fictionally_reserved_names_.push_back(result_set->getString("name"));
	}

	statement.reset(conn->prepareStatement("SELECT * FROM `name_racially_inappropriate`;"));
	result_set.reset(statement->executeQuery());
	while(result_set->next())
	{
        racially_inappropriate_.push_back(result_set->getString("name"));
	}

	statement.reset(conn->prepareStatement("SELECT * FROM `name_developer`;"));	
	result_set.reset(statement->executeQuery());
	while(result_set->next())
	{
		developer_names_.push_back(result_set->getString("name"));
	}
}

vector<CharacterData> MysqlCharacterProvider::GetCharactersForAccount(uint64_t account_id) {
    vector<CharacterData> characters;

    try {
        auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
        auto statement = std::shared_ptr<sql::PreparedStatement>(
            conn->prepareStatement("CALL sp_ReturnAccountCharacters(?);")
            );
        statement->setInt64(1, account_id);
        auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());

        uint16_t chars_count = result_set->rowsCount();

        if (chars_count > 0)
        {
            // this is needed to ensure we don't get commands out of sync errors
            while (result_set->next())
            {
                CharacterData character;
                character.character_id = result_set->getUInt64("id");

                string custom_name = result_set->getString("custom_name");
                character.name = std::wstring(custom_name.begin(), custom_name.end());

                std::string non_shared_template = result_set->getString("iff_template");
				if (non_shared_template.size() > 30)
				{
					non_shared_template.erase(23, 7);
				}

                character.race_crc = anh::memcrc(non_shared_template);
                character.galaxy_id = kernel_->GetServiceDirectory()->galaxy().id();
                character.status = 1;
                characters.push_back(character);
            } while (statement->getMoreResults());
        }
    } catch(sql::SQLException &e) {
        LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
        LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
    }

    return characters;
}

bool MysqlCharacterProvider::DeleteCharacter(uint64_t character_id, uint64_t account_id){
    // this actually just archives the character and all their data so it can still be retrieved at a later time
    string sql = "CALL sp_CharacterDelete(?,?);";
    int rows_updated = 0;
    try {
        auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
        auto statement = shared_ptr<sql::PreparedStatement>(conn->prepareStatement(sql));
        statement->setUInt64(1, character_id);
        statement->setUInt64(2, account_id);
        auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());
        if (result_set->next())
        {
           rows_updated = result_set->getInt(1);
        }
    }
     catch(sql::SQLException &e) {
        LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
        LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
    }
    return rows_updated > 0;
}
std::wstring MysqlCharacterProvider::GetRandomNameRequest(const std::string& base_model) {
    try {
        auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
        auto statement = std::shared_ptr<sql::PreparedStatement>(
            conn->prepareStatement("CALL sp_CharacterNameCreate(?);")
            );
        statement->setString(1, base_model);
        auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());
        if (result_set->next())
        {
            std::string str = result_set->getString(1);
            std::wstring wstr(str.begin(), str.end());
            return wstr;
        }
    } catch(sql::SQLException &e) {
        LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
        LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
    }
    return L"";
}
uint16_t MysqlCharacterProvider::GetMaxCharacters(uint64_t player_id) {
    uint16_t max_chars = 2;
    try {
        auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
        auto statement = std::shared_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT max_characters from player_account where id = ?")
            );
        statement->setUInt64(1, player_id);
        auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());
        if (result_set->next())
        {
            max_chars = result_set->getUInt(1);
        }
    } catch(sql::SQLException &e) {
        LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
        LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
    }
    return max_chars;
}

tuple<uint64_t, string> MysqlCharacterProvider::CreateCharacter(const ClientCreateCharacter& character_info, uint32_t account_id) {
    try {
        // A regular expression that searches for a first name and optional sirname.
        // Only letters, and the ' and - characters are allowed. Only 3 instances
        // of the ' and - characters may be in the entire name, which must be between
        // 3 and 16 characters long.
        const wregex p(
            L"^(?!['-])" // confirm the first character is not ' or -
            L"(?=([^'-]*['-]){0,3}[^'-]*$)" // Confirm that no more than 3 instances of ' or - appear
            L"([a-zA-Z][a-z'-]{2,15})"  // Firstname capture group: 3-16 chars must be a-zA-Z or ' or -
            L"(\\s([a-zA-Z][a-z'-]{2,15}))?$"  // Optional sirname group, same restrictions as sirname
        );

        wsmatch m;

        if (! regex_match(character_info.character_name, m, p)) {
            LOG(warning) << "Invalid character name [" << std::string(character_info.character_name.begin(), character_info.character_name.end()) << "]";
            return make_tuple(0,"name_declined_syntax");
        }

        std::wstring first_name = m[2].str();
        std::wstring last_name = m[4].str();

        std::wstring custom_name = first_name;
        if (!last_name.empty())
        {
            custom_name += L" " + last_name;
        }

        auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");

        std::unique_ptr<sql::PreparedStatement> statement(conn->prepareStatement(
            "CALL sp_CharacterCreate(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, @output)"));

        LOG(warning) << "Creating character with location " << account_id;

        statement->setUInt(1, account_id);
        statement->setUInt(2, kernel_->GetServiceDirectory()->galaxy().id());
        statement->setString(3, string(first_name.begin(), first_name.end()));
        statement->setString(4, string(last_name.begin(), last_name.end()));
        statement->setString(5, string(custom_name.begin(), custom_name.end()));
        statement->setString(6, character_info.starting_profession);
        statement->setString(7, character_info.start_location);
        statement->setDouble(8, character_info.height);
        statement->setString(9, character_info.biography);
        statement->setString(10, character_info.character_customization);
        statement->setString(11, character_info.hair_object);
        statement->setString(12, character_info.hair_customization);
        statement->setString(13, character_info.player_race_iff);

        statement->execute();

        statement.reset(conn->prepareStatement("SELECT @output as _object_id"));

        auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());
        if (result_set->next())
        {
            uint64_t char_id = result_set->getUInt64(1);
            if (char_id < 1002)
            {
                // if we get a special character_id back it means there was an error.
                /// @TODO Change this to return a separate output value for the error code
                return make_tuple(0, setCharacterCreateErrorCode_(static_cast<uint32_t>(char_id)));
            }
            return make_tuple(char_id, "");
        }
    }
    catch(sql::SQLException &e)
    {
        LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
        LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
    }

    return make_tuple(0, "name_declined_internal_error");
}

std::string MysqlCharacterProvider::setCharacterCreateErrorCode_(uint32_t error_code)
{
    std::string error_string;
    switch(error_code)
    {
    case 0:
        error_string = "name_declined_developer";
        break;

    case 1:
        error_string = "name_declined_empty";
        break;

    case 2:
        error_string = "name_declined_fictionally_reserved";
        break;

    case 3:
        error_string = "name_declined_in_use";
        break;

    case 4:
        error_string = "name_declined_internal_error";
        break;

    case 5:
        error_string = "name_declined_no_name_generator";
        break;

    case 6:
        error_string = "name_declined_no_template";
        break;

    case 7:
        error_string = "name_declined_not_authorized_for_species";
        break;

    case 8:
        error_string = "name_declined_not_creature_template";
        break;

    case 9:
        error_string = "name_declined_not_authorized_for_species";
        break;

    case 10:
        error_string = "name_declined_number";
        break;

    case 11:
        error_string = "name_declined_profane";
        break;

    case 12:
        error_string = "name_declined_racially_inappropriate";
        break;

    case 13:
        error_string = "name_declined_reserved";
        break;

    case 14:
        error_string = "name_declined_retry";
        break;

    case 15:
        error_string = "name_declined_syntax";
        break;

    case 16:
        error_string = "name_declined_too_fast";
        break;

    case 17:
        error_string = "name_declined_cant_create_avatar";
        break;

    default:
        error_string = "name_declined_internal_error";
        break;
    }
    return error_string;
}

uint64_t MysqlCharacterProvider::GetCharacterIdByName(const string& name)
{
    uint64_t character_id = 0;
    try {
        auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
        auto statement = std::unique_ptr<sql::PreparedStatement>(
            conn->prepareStatement("SELECT id FROM object where custom_name like ? and type_id = ?;")
            );
        statement->setString(1, name + '%');
        statement->setUInt(2, swganh::object::player::Player::type);
        auto result_set = std::unique_ptr<sql::ResultSet>(statement->executeQuery());
        if (result_set->next())
        {
           character_id = result_set->getUInt64(1);
        }

    } catch(sql::SQLException &e) {
        LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
        LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
    }
    return character_id;
}

std::tuple<bool, std::string> MysqlCharacterProvider::IsNameAllowed(std::string name)
{
	// Convert name to lower-case.
	boost::to_lower(name);

	// Run name through filters.
	for(std::string restricted_name : profane_names_) {
		if(regex_search(name, regex(restricted_name)))
			return std::tuple<bool, std::string>(false, "name_declined_profane");
	}

	for(std::string restricted_name : reserved_names_) {
		if(regex_search(name, regex(restricted_name)))
			return std::tuple<bool, std::string>(false, "name_declined_reserved");
	}

	for(std::string restricted_name : fictionally_reserved_names_) {
		if(regex_search(name, regex(restricted_name)))
			return std::tuple<bool, std::string>(false, "name_declined_fictionally_reserved");
	}

	for(std::string restricted_name : racially_inappropriate_) {
		if(regex_search(name, regex(restricted_name)))
			return std::tuple<bool, std::string>(false, "name_declined_racially_inappropriate");
	}

	for(std::string restricted_name : developer_names_) {
		if(regex_search(name, regex(restricted_name)))
			return std::tuple<bool, std::string>(false, "name_declined_developer");
	}
	
	return std::tuple<bool, std::string>(true, " ");
}