/*

@startuml
hide empty description

[*] --> Idle
Idle --> Calibration_IR_Robot : calibration_start

state Calibration_IR_Robot {

InitStartCalibration : E: send_display_calibration_ihm\nE: send_go_to_home_robot
ComputeTransform: E: compute_transform
SnapshotReplyWaiting: X: find_bevel
SnapshotRequest: E: send_snapshot_request
MoveToCalibrationPointWaiting: E: remove_calibration_point
MoveToCalibrationPoint: E: send_to_go_point
GoToLoadingPose: E: send_go_to_loading_pose
LoadConfirmation: E: send_load_confirmation_ihm

[*] --> InitStartCalibration
InitStartCalibration --> WaitingInitCalibrationRobot : ack_display_ihm
InitStartCalibration --> WaitingInitCalibrationIHM : ack_home_pose_robot
WaitingInitCalibrationRobot --> GoToLoadingPose : ack_home_pose_robot
WaitingInitCalibrationIHM --> GoToLoadingPose : ack_display_ihm
GoToLoadingPose --> GoToLoadingPoseWaiting
GoToLoadingPoseWaiting --> LoadConfirmation : ack_load_pose_robot
LoadConfirmation --> LoadConfirmationWaiting
LoadConfirmationWaiting --> CheckCalibrationPoint : ack_load_confirmation_ihm
CheckCalibrationPoint --> MoveToCalibrationPoint : 1-
CheckCalibrationPoint --> ComputeTransform : 2- [No_More_Points]
MoveToCalibrationPoint --> MoveToCalibrationPointWaiting
MoveToCalibrationPointWaiting --> SnapshotRequest : ack_move_point_robot
SnapshotRequest --> SnapshotReplyWaiting
SnapshotReplyWaiting --> CheckCalibrationPoint : ack_snapshot
ComputeTransform --> Idle
}

@enduml

*/

#include "Sequencer/Sequencer.hpp"
#include "Sequencer/Timer.hpp"

#include <iostream>
#include <string>

using namespace std;

int main()
{
    Sequencer sequencer;

    std::string command;
    do {
        cout << "s: start_calibration\n";
        cout << "ai: ack display ihm\n";
        cout << "ar: ack home pose robot\n";
        cout << "alp: ack load pose robot\n";
        cout << "alc: ack load confirm ihm\n";
        cout << "amp: ack move to point\n";
        cout << "as: ack snapshot request\n";
        cout << "t: timeout in 2s\n";
        cout << "c: cancel timer\n";
        cout << "r: reset\n";
        cout << "q: quit\n";
        cout << "\n> ";
        cout.flush();

        if (!getline(cin, command)) {
            break;
        }

        if (command == "s") {
            sequencer.put(calibration_start{});
        }
        else if (command == "r") {
            sequencer.reset();
        }
        else if (command == "t") {
            sequencer.startTimer(std::chrono::seconds{2});
        }
        else if (command == "c") {
            sequencer.cancelTimer();
        }
        else if (command == "ai") {
            sequencer.put(ack_display_ihm{});
        }
        else if (command == "ar") {
            sequencer.put(ack_home_pose_robot{});
        }
        else if (command == "alp") {
            sequencer.put(ack_load_pose_robot{});
        }
        else if (command == "alc") {
            sequencer.put(ack_load_confirmation_ihm{});
        }
        else if (command == "amp") {
            sequencer.put(ack_move_point_robot{});
        }
        else if (command == "as") {
            sequencer.put(ack_snapshot{});
        }

    } while (command != "q");
}
