#ifndef SWGANH_OBJECT_PLAYER_PLAYER_H_
#define SWGANH_OBJECT_PLAYER_PLAYER_H_

#include <cstdint>
#include <array>
#include <string>
#include <list>

#include "anh/crc.h"

#include "swganh/object/object.h"
#include "swganh/object/waypoint/waypoint.h"

#include "swganh/messages/containers/network_sorted_list.h"
#include "swganh/messages/containers/network_array.h"
#include "swganh/messages/containers/network_map.h"
#include "swganh/messages/containers/network_sorted_vector.h"

namespace swganh {
namespace object {
namespace player {

enum Gender
{
    FEMALE = 0,
    MALE
};

enum StatusFlags
{
    CLEAR           = 0,
    LFG             = 0x00000001,
    HELPER          = 0x00000002,
    ROLEPLAYER      = 0x00000004,
    AFK             = 0x00000080,
    LD              = 0x00000100,
    FACTION_RANK    = 0x00000200,
    ANONYMOUS       = 0x80000000
};
enum StatusIndex
{
    DEFAULT = 0,
    ANON = 4
};
//@TODO: discover these
enum ProfileFlags
{
    LIKES_X = 1,
    DISLIKES_X
};

enum AdminTag
{
    CSR = 1,
    DEVELOPER = 2
};

struct Ability
{
    Ability(void)
        : ability("")
    {}

    Ability(std::string value_)
        : ability(value_)
    {}

    ~Ability()
    {}

    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<std::string>(ability);
    }

    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write<std::string>(ability);
    }
    bool operator==(const Ability& other)
    {
        return ability == other.ability;
    }

    std::string ability;
};
struct Name
{
    Name(void)
        : name("")
    {}

    Name(const std::string& value_)
        : name(value_)
    {}

    ~Name()
    {}

    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<std::string>(name);
    }

    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write<std::string>(name);
    }
    bool operator==(const Name& other)
    {
        return name == other.name;
    }

    std::string name;
};
struct XpData
{
    XpData()
        : type("")
        , value(0){}
    XpData(std::string in_type, uint32_t in_value)
    : type(in_type)
    , value(in_value){}

    std::string type;
    uint32_t value;

    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<uint8_t>(0); // byte
        message.data.write(type);
        message.data.write(value);           
    }
    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write(type);
        message.data.write(value);
    }
    bool operator==(const XpData& other)
    {
        return (type == other.type && value == other.value);
    }
};
struct FlagBitmask
{
    FlagBitmask()
        : bitmask(0)
    {}

    FlagBitmask(uint32_t flag_bitmask_)
        : bitmask(flag_bitmask_)
    {}

    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<uint32_t>(bitmask);
    }

    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write<uint32_t>(bitmask);
    }

    bool operator==(const FlagBitmask& other)
    {
        return other.bitmask == bitmask;
    }

    uint32_t bitmask;
};

struct QuestJournalData
{
    QuestJournalData()
        : quest_crc(0)
        , owner_id(0)
        , active_step_bitmask(0)
        , completed_step_bitmask(0)
        , completed_flag(false)
        , counter(0){}

    uint32_t quest_crc;
    uint64_t owner_id;
    uint16_t active_step_bitmask;
    uint16_t completed_step_bitmask;
    bool completed_flag;
    uint32_t counter;

    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<uint8_t>(0); // byte
        message.data.write<uint32_t>(quest_crc);
        message.data.write<uint64_t>(owner_id);
        message.data.write<uint16_t>(active_step_bitmask);
        message.data.write<uint16_t>(completed_step_bitmask);
        if (completed_flag)
            message.data.write<uint8_t>(1);
        else
            message.data.write<uint8_t>(0);
        message.data.write<uint32_t>(counter);
    }

    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write<uint32_t>(quest_crc);
        message.data.write<uint64_t>(owner_id);
        message.data.write<uint16_t>(active_step_bitmask);
        message.data.write<uint16_t>(completed_step_bitmask);
        if (completed_flag)
            message.data.write<uint8_t>(1);
        else
            message.data.write<uint8_t>(0);
        message.data.write<uint32_t>(counter);
    }

    bool operator==(const QuestJournalData& other)
    {
        return (quest_crc == other.quest_crc);
    }
};

struct DraftSchematicData
{
    DraftSchematicData()
        : schematic_id(0)
        , schematic_crc(0)
    {}

    DraftSchematicData(uint32_t schematic_id_)
        : schematic_id(schematic_id_)
        , schematic_crc(0)
    {}

    DraftSchematicData(uint32_t schematic_id_, uint32_t schematic_crc_)
        : schematic_id(schematic_id_)
        , schematic_crc(schematic_crc_)
    {}

    ~DraftSchematicData()
    {}

    uint32_t schematic_id;
    uint32_t schematic_crc;

    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<uint32_t>(schematic_id);
        message.data.write<uint32_t>(schematic_crc);
    }

    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write<uint32_t>(schematic_id);
        message.data.write<uint32_t>(schematic_crc);
    }

    bool operator==(const DraftSchematicData& other)
    {
        return schematic_id == other.schematic_id;
    }
};
struct WaypointData
{
    WaypointData()
        : object_id_(0)
        , cell_id_(0)
        , coordinates_(glm::vec3(0,0,0))
        , location_network_id_(0)
        , planet_name_("")
        , name_(L"")
        , color_(0)
        , activated_flag_(0) {}

    WaypointData(uint64_t object_id, uint32_t cell_id, glm::vec3 coords, uint64_t network_id, std::string planet_name,
        std::wstring name, uint8_t color, uint8_t activated_flag)
        : object_id_(object_id)
        , cell_id_(cell_id)
        , coordinates_(std::move(coords))
        , location_network_id_(network_id)
        , planet_name_(planet_name)
        , name_(name)
        , color_(color)
        , activated_flag_(activated_flag) {}
    
    void Serialize(swganh::messages::BaselinesMessage& message)
    {
        message.data.write<uint64_t>(object_id_);
        message.data.write<uint32_t>(cell_id_);
        message.data.write<float>(coordinates_.x);
        message.data.write<float>(coordinates_.y);
        message.data.write<float>(coordinates_.z);
        message.data.write<uint64_t>(location_network_id_);
        message.data.write<uint32_t>(anh::memcrc(planet_name_));
        message.data.write<std::wstring>(name_);
        message.data.write<uint64_t>(object_id_);
        message.data.write<uint8_t>(color_);
        message.data.write<uint8_t>(activated_flag_);
    }

    void Serialize(swganh::messages::DeltasMessage& message)
    {
        message.data.write<uint64_t>(object_id_);
        message.data.write<uint32_t>(cell_id_);
        message.data.write<float>(coordinates_.x);
        message.data.write<float>(coordinates_.y);
        message.data.write<float>(coordinates_.z);
        message.data.write<uint64_t>(location_network_id_);
        message.data.write<uint32_t>(anh::memcrc(planet_name_));
        message.data.write<std::wstring>(name_);
        message.data.write<uint64_t>(object_id_);
        message.data.write<uint8_t>(color_);
        message.data.write<uint8_t>(activated_flag_);
    }

    bool operator==(const WaypointData& other)
    {
        return object_id_ == other.object_id_;
    }

    uint64_t object_id_;
    uint32_t cell_id_;
    glm::vec3 coordinates_;
    uint64_t location_network_id_;
    std::string planet_name_;
    std::wstring name_;
    uint8_t not_used_;
    uint8_t color_;
    uint8_t activated_flag_;
};

class PlayerMessageBuilder;

class Player : public swganh::object::Object
{
public:
    Player();
    
    /**
     * @return The type of the object.
     */    
    virtual uint32_t GetType() const { return Player::type; }
    const static uint32_t type = 0x504c4159;

    /**
     * @return The status flags for the player object.
     */
    std::array<FlagBitmask, 4> GetStatusFlags();

    /**
     * Adds a flag to the status flags.
     * [INT1]
     * 1 - LFG(can combine with HELPER)
     * 2 - HELPER(can combine with LFG)
     * 4 - ROLEPLAYER (no tag)
     * 80 - AFK (overrides previous tags)
     * 100 - LD (overrides other tags)
     * 200 - Faction Rank(seperate tag)
     * 
     * [INT4]
     * 80000000 - Anonymous (no tag) 
     * 
     * @param flag The value of the flag.
     * @param index The index of the flag (generally just left at default).
     */
    void AddStatusFlag(StatusFlags flag, StatusIndex index = DEFAULT);

    /**
     * Removes a status flag
     *
     * @param flag The value of the flag.
     * @param index The index of the flag (generally just left at default).
     */
    void RemoveStatusFlag(StatusFlags flag, StatusIndex index = DEFAULT);

    /**
     * Clears all status flags.
     */
    void ClearStatusFlags();
    
    /**
     * @return The profile flags for the player object.
     */
    std::array<FlagBitmask, 4> GetProfileFlags();
    
    /**
     * Adds a flag to the profile flags.
     *
     * @param flag The value of the flag.
     * @param index The index of the flag (generally just left at default).
     */
    void AddProfileFlag(ProfileFlags flag, StatusIndex index = DEFAULT);
    
    /**
     * Removes a profile flag
     *
     * @param flag The value of the flag.
     * @param index The index of the flag (generally just left at default).
     */
    void RemoveProfileFlag(ProfileFlags flag, StatusIndex index = DEFAULT);
    
    /**
     * Clears all profile flags.
     */
    void ClearProfileFlags();
    
    /**
     * @return The profession tag currently set for the player.
     */
    std::string GetProfessionTag() ;
    
    /**
     * Sets the profession tag for the player.
     *
     * @param profession_tag The profession tag name.
     */
    void SetProfessionTag(std::string profession_tag);

    /**
     * @return the date the character was created.
     */
    uint32_t GetBornDate() ;

    /**
     * Sets the born date for the character.
     *
     * The integer of the time when the character was created to be used 
     * as born date. This has been implemented by simply using yyyymmdd.
     * Client ignores - / (spaces) in this, so a simple date without any 
     * characters makes this work. 
     *
     * Example: 20080110
     *
     * @param born_date The born date for the character.
     */
    void SetBornDate(uint32_t born_date);

    /**
     * @return the total playtime for the player.
     */
    uint32_t GetTotalPlayTime() ;

    /**
     * Sets the total playtime for the player.
     *
     * @param play_time the new play time for the player.
     */
    void SetTotalPlayTime(uint32_t play_time);


    /**
     * Increments the play time by the specified amount.
     *
     * @param increment the amount of play time to increment by.
     */
    void IncrementTotalPlayTime(uint32_t increment);
    
    /**
     * @return the admin tag.
     */
    uint8_t GetAdminTag() ;

    /**
     * Sets the admin tag, can be one of the following:
     *
     * 0 = no tag
     * 1 = CSR
     * 2 = Developer
     *
     * @param tag The new admin tag.
     *
     * @TODO consider making this an enum.
     */
    void SetAdminTag(uint8_t tag);
    
    /**
     * @return the current experience for the player.
     */
    swganh::messages::containers::NetworkMap<std::string, XpData> GetXp() ;
    
    /**
     * Adds experience to the player.
     *
     * @param experience The experience to add.
     */
    void AddExperience(XpData experience);
    
    /**
     * Removes experience from the player.
     *
     * @param experience The experience to remove.
     */
    void DeductXp(XpData experience);

    /**
     * Clears all experience of a specific type.
     *
     * @param type The type of experience to clear.
     */
    void ClearXpType(std::string type);
    
    /**
     * Resets the experience of the player.
     *
     * @param experience The new experience counts for the player.
     */
    void ResetXp(swganh::messages::containers::NetworkMap<std::string, XpData>& experience);
    
    /**
     * Clears all experience.
     */
    void ClearAllXp();
    
    /**
     * @return The waypoints currently held by the player.
     */
    swganh::messages::containers::NetworkMap<uint64_t, WaypointData> GetWaypoints() ;
    
    /**
     * Adds a waypoint to the player.
     *
     * @param waypoint The waypoint to add to the player.
     */
    void AddWaypoint(WaypointData waypoint);
    
    /**
     * Removes a waypoint from the player.
     * 
     * @param waypoint_id The id of the waypoint to remove.
     */
    void RemoveWaypoint(uint64_t waypoint_id);

    /**
     * Modifies an existing waypoint.
     *
     * @param waypoint The new waypoint data.
     */
    void ModifyWaypoint(WaypointData waypoint);
    
    /** 
     * Clears all waypoints.
     */
    void ClearAllWaypoints();

    /**
     * @return the current force power of the player.
     */
    uint32_t GetCurrentForcePower() ;
    
    /**
     * Increments the force power of the player. Can be a negative value.
     *
     * @param force_power The amount of force to increment.
     */
    void IncrementForcePower(int32_t force_power);
    
    /**
     * Sets the current force power.
     *
     * @param force_power The new force power.
     */
    void SetCurrentForcePower(uint32_t force_power);
    
    /**
     * @return the max force power of the player.
     */
    uint32_t GetMaxForcePower() ;
    
    /**
     * Sets the max force power.
     *
     * @param force_power The new max force power.
     */
    void SetMaxForcePower(uint32_t force_power);

    uint32_t GetCurrentForceSensitiveQuests();
    void AddCurrentForceSensitiveQuest(uint32_t quest_mask);
    void RemoveCurrentForceSensitiveQuest(uint32_t quest_mask);
    void ClearCurrentForceSensitiveQuests();
    
    uint32_t GetCompletedForceSensitiveQuests();
    void AddCompletedForceSensitiveQuest(uint32_t quest_mask);
    void RemoveCompletedForceSensitiveQuest(uint32_t quest_mask);
    void ClearCompletedForceSensitiveQuests();

    /**
     * @return The quests currently in the quest journal.
     */
    swganh::messages::containers::NetworkMap<uint32_t, QuestJournalData> GetQuests() ;
    
    /**
     * Adds a quest to the journal.
     *
     * @param quest the quest to add.
     */
    void AddQuest(QuestJournalData quest);
    
    /**
     * Removes a quest from the journal.
     *
     * @param quest to remove.
     */
    void RemoveQuest(QuestJournalData quest);
    
    /**
     * Updates a quest in the journal.
     * 
     * @param quest to update.
     */
    void UpdateQuest(QuestJournalData quest);

    /**
     * Clears all quests in the journal.
     */
    void ClearAllQuests();
    
    /**
     * @return list of abilities and certificates this player has. 
     */
    swganh::messages::containers::NetworkSortedList<Ability> GetAbilityList() ;

    /**
     * Checks to see if the player has the input ability
     *
     * @param ability to check for
     * @return bool true if the ability has been found
     */
    bool HasAbility(std::string ability);
    
    /**
     * @return whether player can experiment or not.
     */
    uint32_t GetExperimentationFlag() ;

    /**
     * Sets the experimentation flag for the player.
     *
     * @param experimentation_flag The new experimentation flag.
     *
     * @TODO Consider making this interface boolean.
     */
    void SetExperimentationFlag(uint32_t experimentation_flag);
    
    /**
     * @return The current crafting stage of the player's crafting session.
     */
    uint32_t GetCraftingStage() ;
    
    /**
     * Sets the crafting stage for the player.
     *
     * @param crafting_stage the new crafting stage.
     */
    void SetCraftingStage(uint32_t crafting_stage);

    /**
     * @return the ID of the nearest crafting station.
     */
    uint64_t GetNearestCraftingStation() ;
    
    /**
     * Sets the id for the nearest crafting station.
     *
     * @param crafting_station_id ID of the nearest crafting station.
     */
    void SetNearestCraftingStation( uint64_t crafting_station_id);
    
    /**
     * @return The draft schematics assigned to this player.
     */
    swganh::messages::containers::NetworkSortedList<DraftSchematicData> GetDraftSchematics() ;
    
    /**
     * Adds a draft schematic.
     *
     * @param schematic the schematic to add.
     */
    void AddDraftSchematic(DraftSchematicData schematic);
    
    /**
     * Removes a draft schematic.
     *
     * @param schematic_id The id of the schematic to remove.
     */
    void RemoveDraftSchematic(uint32_t schematic_id);
    
    /**
     * Clears all draft schematics.
     */
    void ClearDraftSchematics();

    /**
     * @return The number of experimentation points available in the
     *  current crafting session.
     */
    uint32_t GetExperimentationPoints() ;

    /**
     * Adds experimentation points to the current crafting session.
     *
     * @param points experimentation points to add.
     */ 
    void AddExperimentationPoints(uint32_t points);
    
    /**
     * Removes experimentation points from the current crafting session.
     *
     * @param points experimentation points to remove.
     */ 
    void RemoveExperimentationPoints(uint32_t points);
    
    /**
     * Resets experimentation points for the current crafting session.
     *
     * @param points experimentation points to reset to.
     */ 
    void ResetExperimentationPoints(uint32_t points);
    
    /**
     * @return the number of accomplishments attained by this player.
     */
    uint32_t GetAccomplishmentCounter() ;
    
    /**
     * Resets the accomplishment counter.
     *
     * @param counter New accomplishment count.
     */
    void ResetAccomplishmentCounter(uint32_t counter);
    
    /**
     * Increments the accomplishment counter.
     */
    void IncrementAccomplishmentCounter();

    /**
     * @return the list of friends.
     */
    swganh::messages::containers::NetworkSortedVector<Name> GetFriends();
    
    /**
     * Adds a player to the friend list.
     *
     * @param friend_name Name of the friend to add.
     */ 
    void AddFriend(std::string friend_name);
    
    /**
     * Removes a friend from the friend list.
     *
     * @param friend_name Name of the friend to remove.
     */ 
    void RemoveFriend(std::string friend_name);

    /**
     * Clears the friends list.
     */
    void ClearFriends();
    
    /**
     * @return the list of ignored players.
     */
    swganh::messages::containers::NetworkSortedVector<Name> GetIgnoredPlayers();

    /**
     * Adds a player to the ignored list.
     *
     * @param player_name Name of the player to ignore.
     */ 
    void IgnorePlayer(std::string player_name);
    
    /**
     * Removes a player from the ignored list.
     *
     * @param player_name Name of the player to stop ignoring.
     */ 
    void StopIgnoringPlayer(std::string player_name);
    
    /**
     * Clears the ignored list.
     */
    void ClearIgnored();
    
    /**
     * @return the current speaking language of the player.
     */
    uint32_t GetLanguage() ;
    void SetLanguage(uint32_t language_id);
    
    /**
     * @return the current stomach volume of the player.
     */
    uint32_t GetCurrentStomach() ;
    
    /**
     * Increases the current stomach volume of the player.
     *
     * @param stomach Amount to increase the stomach volume.
     */ 
    void IncreaseCurrentStomach(uint32_t stomach);
    
    /**
     * Decreases the current stomach volume of the player.
     *
     * @param stomach Amount to decrease the stomach volume.
     */ 
    void DecreaseCurrentStomach(uint32_t stomach);
    
    /**
     * Resets the current stomach volume of the player.
     *
     * @param stomach The new stomach volume.
     */
    void ResetCurrentStomach(uint32_t stomach);
    
    /**
     * @return the max stomach volume of the player.
     */
    uint32_t GetMaxStomach() ;
    
    /**
     * Resets the max stomach volume of the player.
     *
     * @param stomach The new max stomach volume.
     */
    void ResetMaxStomach(uint32_t stomach);
    
    /**
     * @return the current drink volume of the player.
     */
    uint32_t GetCurrentDrink() ;
    
    /**
     * Increases the current drink volume of the player.
     *
     * @param drink Amount to increase the drink volume.
     */ 
    void IncreaseCurrentDrink(uint32_t drink);
    
    /**
     * Decreases the current drink volume of the player.
     *
     * @param drink Amount to decrease the drink volume.
     */ 
    void DecreaseCurrentDrink(uint32_t drink);

    /**
     * Resets the current drink volume of the player.
     *
     * @param drink The new drink volume.
     */
    void ResetCurrentDrink(uint32_t drink);
    
    /**
     * @return the max drink volume of the player.
     */
    uint32_t GetMaxDrink() ;

    /**
     * Resets the max drink volume of the player.
     *
     * @param drink The new max drink volume.
     */
    void ResetMaxDrink(uint32_t drink);
    
    /**
     * @return the current jedi state of the player.
     */
    uint32_t GetJediState() ;

    /**
     * Sets the current jedi state for the player.
     *
     * @param jedi_state The jedi state for the player.
     */
    void SetJediState(uint32_t jedi_state);
    
    /**
     * @return the gender of the player.
     */
    Gender GetGender() ;

    /**
     * Sets the gender of the player.
     *
     * @param gender The gender of the player.
     */
    void SetGender(Gender gender);

protected:
    // baselines
    virtual boost::optional<swganh::messages::BaselinesMessage> GetBaseline3();
    virtual boost::optional<swganh::messages::BaselinesMessage> GetBaseline6();
    virtual boost::optional<swganh::messages::BaselinesMessage> GetBaseline8();
    virtual boost::optional<swganh::messages::BaselinesMessage> GetBaseline9();

private:
    friend class PlayerMessageBuilder;
	friend class PlayerFactory;

    void SetDeltaBitmask_(uint32_t bitmask, uint16_t update_type, swganh::object::Object::ViewType view_type);

    std::array<FlagBitmask, 4> status_flags_;
    std::array<FlagBitmask, 4> profile_flags_;
    std::string profession_tag_;
    uint32_t born_date_;
    uint32_t total_playtime_;
    uint8_t admin_tag_;
    uint32_t region_;
    swganh::messages::containers::NetworkMap<std::string, XpData> experience_;
    uint32_t experience_counter_;
    swganh::messages::containers::NetworkMap<uint64_t, WaypointData> waypoints_;
    uint32_t waypoint_counter_;
    uint32_t current_force_power_;
    uint32_t max_force_power_;
    uint32_t current_force_sensitive_quests_;
    uint32_t completed_force_sensitive_quests_;
    swganh::messages::containers::NetworkMap<uint32_t, QuestJournalData> quest_journal_;
    uint32_t experimentation_flag_;
    uint32_t crafting_stage_;
    uint64_t nearest_crafting_station_;
    swganh::messages::containers::NetworkSortedList<DraftSchematicData> draft_schematics_;
    uint32_t experimentation_points_;
    uint32_t accomplishment_counter_;
    swganh::messages::containers::NetworkSortedVector<Name> friends_;
    swganh::messages::containers::NetworkSortedVector<Name> ignored_players_;
    uint32_t language_;
    uint32_t current_stomach_;
    uint32_t max_stomach_;
    uint32_t current_drink_;
    uint32_t max_drink_;
    uint32_t jedi_state_;
    Gender gender_;
};

}}}  // namespace swganh::object::player

#endif  // SWGANH_OBJECT_PLAYER_PLAYER_H_
