#include <gtest/gtest.h>
#include "Device.h"

TEST(DeviceTests, StartsInIdleState)
{
    Device device;

    DeviceStatus status = device.getStatus();

    EXPECT_EQ(status.state, DeviceState::IDLE);
    EXPECT_FALSE(status.motorRunning);
    EXPECT_FALSE(status.conveyorRunning);
}

TEST(DeviceTests, StartTurnsOnMotorAndConveyor)
{
    Device device;

    device.start();

    DeviceStatus status = device.getStatus();

    EXPECT_EQ(status.state, DeviceState::RUNNING);
    EXPECT_TRUE(status.motorRunning);
    EXPECT_TRUE(status.conveyorRunning);
}

TEST(DeviceTests, StopTurnsOffMotorAndConveyor)
{
    Device device;

    device.start();
    device.stop();

    DeviceStatus status = device.getStatus();

    EXPECT_EQ(status.state, DeviceState::IDLE);
    EXPECT_FALSE(status.motorRunning);
    EXPECT_FALSE(status.conveyorRunning);
}

TEST(DeviceTests, EmergencyStopTurnsOffAllActuators)
{
    Device device;

    device.start();
    device.emergencyStop();

    DeviceStatus status = device.getStatus();

    EXPECT_EQ(status.state, DeviceState::EMERGENCY_STOP);
    EXPECT_FALSE(status.motorRunning);
    EXPECT_FALSE(status.conveyorRunning);
}

TEST(DeviceTests, ResetRecoversFromEmergencyStop)
{
    Device device;

    device.start();
    device.emergencyStop();
    device.reset();

    DeviceStatus status = device.getStatus();

    EXPECT_EQ(status.state, DeviceState::IDLE);
    EXPECT_FALSE(status.motorRunning);
    EXPECT_FALSE(status.conveyorRunning);
    EXPECT_TRUE(status.errorMessage.empty());
}

TEST(DeviceTests, TemperatureIncreasesWhenRunning)
{
    Device device;

    const double initialTemperature =
        device.getStatus().temperature;

    device.start();
    device.update();

    const double updatedTemperature =
        device.getStatus().temperature;

    EXPECT_GT(updatedTemperature, initialTemperature);
}