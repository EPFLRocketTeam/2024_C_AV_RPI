#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include "av_state.h"
#include "data.h"
#include "intranet_commands.h"
#include "PR_board.h"

// Test helper class that mimics the check_policy logic
class PR_boardLogicTester {
public:
    // Flags to track which code paths are executed
    bool init_executed = false;
    bool errorground_executed = false;
    bool descent_executed = false;
    bool manual_executed = false;
    bool errorflight_executed = false;
    bool armed_executed = false;
    bool ready_executed = false;
    bool thrustsequence_executed = false;
    bool default_executed = false;
    
    // Manual mode command flags
    bool abort_executed = false;
    bool deploy_executed = false;
    bool unknown_command_executed = false;
    bool no_command_executed = false;
    
    // Track last processed data
    DataDump last_dump;
    uint32_t last_delta_ms = 0;
    
    void resetFlags() {
        init_executed = false;
        errorground_executed = false;
        descent_executed = false;
        manual_executed = false;
        errorflight_executed = false;
        armed_executed = false;
        ready_executed = false;
        thrustsequence_executed = false;
        default_executed = false;
        abort_executed = false;
        deploy_executed = false;
        unknown_command_executed = false;
        no_command_executed = false;
    }
    
    // Replicates the logic from the check_policy method
    void testCheckPolicyLogic(const DataDump& dump, const uint32_t delta_ms) {
        last_dump = dump;
        last_delta_ms = delta_ms;
        
        switch (dump.av_state) {
            case State::INIT:
                init_executed = true;
                // For the INIT state we do nothing
                break;
                
            case State::ERRORGROUND:
                errorground_executed = true;
                // handleErrorGround(dump) would be called here
                break;
                
            case State::DESCENT:
                descent_executed = true;
                // handleDescent(dump) would be called here
                break;
                
            case State::MANUAL:
                manual_executed = true;
                // processManualMode(dump) logic:
                if (dump.event.command_updated) {
                    switch (dump.telemetry_cmd.id) {
                        case CMD_ID::AV_CMD_ABORT:
                            abort_executed = true;
                            // executeAbort() would be called here
                            break;
                        case CMD_ID::AV_CMD_MANUAL_DEPLOY:
                            deploy_executed = true;
                            // deployRecoverySystem() would be called here
                            break;
                        default:
                            unknown_command_executed = true;
                            break;
                    }
                } else {
                    no_command_executed = true;
                }
                break;
                
            case State::ERRORFLIGHT:
                errorflight_executed = true;
                // handleErrorFlight(dump) would be called here
                break;
                
            case State::ARMED: {
                armed_executed = true;
                // FIXME: this logic is only valid for the FT
                bool prb_ready = true;
                // Data::get_instance().write(Data::EVENT_PRB_READY, &prb_ready) would be called here
                // For the ARMED state we do nothing else
                break;
            }
            
            case State::READY:
                ready_executed = true;
                // processReadyState(dump) would be called here
                break;
                
            case State::THRUSTSEQUENCE:
                thrustsequence_executed = true;
                // TODO: Implement thrust sequence logic
                break;
                
            default:
                default_executed = true;
                break;
        }
    }
};

// Test fixture
class PR_boardTest : public ::testing::Test {
protected:
    void SetUp() override {
        tester.resetFlags();
    }
    
    PR_boardLogicTester tester;
    
    // Helper functions
    DataDump createDataDump(State state) {
        DataDump dump;
        std::memset(&dump, 0, sizeof(dump));
        dump.av_state = state;
        // Initialize other fields to default values
        dump.av_timestamp = 0;
        dump.telemetry_cmd.id = static_cast<CMD_ID>(0);
        dump.telemetry_cmd.value = 0;
        dump.av_fc_temp = 0.0f;
        // Event struct is initialized to false by default constructor
        return dump;
    }
    
    DataDump createDataDumpWithCommand(State state, CMD_ID cmd_id, bool command_updated = true) {
        DataDump dump = createDataDump(state);
        dump.event.command_updated = command_updated;
        dump.telemetry_cmd.id = cmd_id;
        dump.telemetry_cmd.value = 0; // Default value
        return dump;
    }
};

// Test cases for all states defined in your State enum
TEST_F(PR_boardTest, CheckPolicy_InitState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::INIT);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_CalibrationState_ExecutesDefaultPath) {
    DataDump dump = createDataDump(State::CALIBRATION);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    // CALIBRATION is not handled in the switch, so it should hit default
    EXPECT_TRUE(tester.default_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ErrorGroundState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::ERRORGROUND);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.errorground_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_DescentState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::DESCENT);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.descent_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ManualState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::MANUAL);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.manual_executed);
    EXPECT_TRUE(tester.no_command_executed); // command_updated defaults to false
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ErrorFlightState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::ERRORFLIGHT);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.errorflight_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ArmedState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::ARMED);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.armed_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ReadyState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::READY);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.ready_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ThrustSequenceState_ExecutesCorrectPath) {
    DataDump dump = createDataDump(State::THRUSTSEQUENCE);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.thrustsequence_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.default_executed);
}

// Test states that are not handled in switch statement (should go to default)
TEST_F(PR_boardTest, CheckPolicy_LiftoffState_ExecutesDefaultPath) {
    DataDump dump = createDataDump(State::LIFTOFF);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.default_executed);
    EXPECT_FALSE(tester.init_executed);
    EXPECT_FALSE(tester.errorground_executed);
    EXPECT_FALSE(tester.descent_executed);
    EXPECT_FALSE(tester.manual_executed);
    EXPECT_FALSE(tester.errorflight_executed);
    EXPECT_FALSE(tester.armed_executed);
    EXPECT_FALSE(tester.ready_executed);
    EXPECT_FALSE(tester.thrustsequence_executed);
}

TEST_F(PR_boardTest, CheckPolicy_AscentState_ExecutesDefaultPath) {
    DataDump dump = createDataDump(State::ASCENT);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.default_executed);
}

TEST_F(PR_boardTest, CheckPolicy_LandedState_ExecutesDefaultPath) {
    DataDump dump = createDataDump(State::LANDED);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.default_executed);
}

// Manual mode command tests
TEST_F(PR_boardTest, CheckPolicy_ManualMode_AbortCommand_ExecutesCorrectPath) {
    DataDump dump = createDataDumpWithCommand(State::MANUAL, CMD_ID::AV_CMD_ABORT);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.manual_executed);
    EXPECT_TRUE(tester.abort_executed);
    EXPECT_FALSE(tester.deploy_executed);
    EXPECT_FALSE(tester.unknown_command_executed);
    EXPECT_FALSE(tester.no_command_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ManualMode_DeployCommand_ExecutesCorrectPath) {
    DataDump dump = createDataDumpWithCommand(State::MANUAL, CMD_ID::AV_CMD_MANUAL_DEPLOY);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.manual_executed);
    EXPECT_TRUE(tester.deploy_executed);
    EXPECT_FALSE(tester.abort_executed);
    EXPECT_FALSE(tester.unknown_command_executed);
    EXPECT_FALSE(tester.no_command_executed);
}

TEST_F(PR_boardTest, CheckPolicy_ManualMode_CommandNotUpdated_ExecutesCorrectPath) {
    DataDump dump = createDataDumpWithCommand(State::MANUAL, CMD_ID::AV_CMD_ABORT, false);
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.manual_executed);
    EXPECT_TRUE(tester.no_command_executed);
    EXPECT_FALSE(tester.abort_executed);
    EXPECT_FALSE(tester.deploy_executed);
    EXPECT_FALSE(tester.unknown_command_executed);
}

// Test with unknown command 
TEST_F(PR_boardTest, CheckPolicy_ManualMode_UnknownCommand_ExecutesCorrectPath) {
    // Cast to create an unknown command 
    DataDump dump = createDataDumpWithCommand(State::MANUAL, static_cast<CMD_ID>(999));
    
    tester.testCheckPolicyLogic(dump, 100);
    
    EXPECT_TRUE(tester.manual_executed);
    EXPECT_TRUE(tester.unknown_command_executed);
    EXPECT_FALSE(tester.abort_executed);
    EXPECT_FALSE(tester.deploy_executed);
    EXPECT_FALSE(tester.no_command_executed);
}

// Parameter passing tests
TEST_F(PR_boardTest, CheckPolicy_ParametersPassedCorrectly) {
    DataDump dump = createDataDump(State::READY);
    uint32_t delta_ms = 250;
    
    tester.testCheckPolicyLogic(dump, delta_ms);
    
    EXPECT_EQ(tester.last_dump.av_state, State::READY);
    EXPECT_EQ(tester.last_delta_ms, delta_ms);
}

// Integration test with multiple states
TEST_F(PR_boardTest, CheckPolicy_MultipleStatesSequence) {
    std::vector<State> states = {
        State::INIT,
        State::CALIBRATION,
        State::MANUAL,
        State::ARMED,
        State::READY,
        State::LIFTOFF,
        State::THRUSTSEQUENCE,
        State::ASCENT,
        State::DESCENT,
        State::LANDED,
        State::ERRORGROUND,
        State::ERRORFLIGHT
    };
    
    for (State state : states) {
        tester.resetFlags();
        DataDump dump = createDataDump(state);
        tester.testCheckPolicyLogic(dump, 100);
        
        // Verify only the correct flag is set based on your switch statement
        switch (state) {
            case State::INIT:
                EXPECT_TRUE(tester.init_executed) << "Failed for state INIT";
                break;
            case State::ERRORGROUND:
                EXPECT_TRUE(tester.errorground_executed) << "Failed for state ERRORGROUND";
                break;
            case State::DESCENT:
                EXPECT_TRUE(tester.descent_executed) << "Failed for state DESCENT";
                break;
            case State::MANUAL:
                EXPECT_TRUE(tester.manual_executed) << "Failed for state MANUAL";
                EXPECT_TRUE(tester.no_command_executed) << "Failed for state MANUAL - no command";
                break;
            case State::ERRORFLIGHT:
                EXPECT_TRUE(tester.errorflight_executed) << "Failed for state ERRORFLIGHT";
                break;
            case State::ARMED:
                EXPECT_TRUE(tester.armed_executed) << "Failed for state ARMED";
                break;
            case State::READY:
                EXPECT_TRUE(tester.ready_executed) << "Failed for state READY";
                break;
            case State::THRUSTSEQUENCE:
                EXPECT_TRUE(tester.thrustsequence_executed) << "Failed for state THRUSTSEQUENCE";
                break;
            // States not handled in switch should go to default
            case State::CALIBRATION:
            case State::LIFTOFF:
            case State::ASCENT:
            case State::LANDED:
                EXPECT_TRUE(tester.default_executed) << "Failed for state " << static_cast<int>(state);
                break;
            default:
                break;
        }
    }
}

// Test to verify DataDump structure initialization
TEST_F(PR_boardTest, DataDump_CreationAndInitialization) {
    DataDump dump = createDataDump(State::READY);
    
    EXPECT_EQ(dump.av_state, State::READY);
    EXPECT_EQ(dump.av_timestamp, 0U);
    EXPECT_EQ(dump.telemetry_cmd.value, 0);
    EXPECT_EQ(dump.av_fc_temp, 0.0f);
    EXPECT_FALSE(dump.event.command_updated); // Should be false by default
}

// Test actual PR_board functionality (if you want to test the real class)
TEST_F(PR_boardTest, ActualPR_board_CheckPolicy_DoesNotCrash) {
    PR_board actual_pr_board;
    DataDump dump = createDataDump(State::INIT);
    
    // This tests that the actual method doesn't crash and completes execution
    EXPECT_NO_THROW(actual_pr_board.check_policy(dump, 100));
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}