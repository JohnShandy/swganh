// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef ANH_APP_APP_INTERFACE_H_
#define ANH_APP_APP_INTERFACE_H_

namespace anh {
namespace app {

class KernelInterface;

/*!
 * @Provides a simple application interface
 */
class AppInterface {
public:
    virtual ~AppInterface() {}

    /*!
     * @Brief Starts up the application
     */
    virtual void Initialize(int argc, char* argv[]) = 0;

    virtual void Start() = 0;

    virtual void Stop() = 0;

    virtual bool IsRunning() = 0;

    virtual KernelInterface* GetAppKernel() const = 0;
};

}}  // namespace anh::app

#endif  // ANH_APP_APP_INTEFACE_H_
