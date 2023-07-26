# State Machine using Actions

```plantuml
@startuml
[*] --> Off 
Off --> On : enable / EnableMotor()

state On {
    [*] --> Idle
    Idle --> Busy : rotate_step(steps) / RotateStepMotor
    Idle --> BusyContinuous : rotate_continuous / RotateContinuousMotor
    Busy --> Idle : stop / StopMotor
    Busy --> Busy : step_done / StepDone
    Busy --> Idle : step_done [NoMoreSteps] / StopMotor
    BusyContinuous --> Idle : stop / StopMotor
  
    On --> Off : disable / DisableMotor()
}
@enduml
```

```
----- process event  --> enable
+enableMotor
-- on entry: Off --> Idle
-- on entry: Off --> Idle
----- process event  --> rotate_step
+rotateStep
-- on entry: Idle --> Busy
----- process event  --> step_done
-- on exit: Busy --> Busy
+stepDone: 3
-- on entry: Busy --> Busy
----- process event  --> step_done
-- on exit: Busy --> Busy
+stepDone: 2
-- on entry: Busy --> Busy
----- process event  --> step_done
-- on exit: Busy --> Busy
+stepDone: 1
-- on entry: Busy --> Busy
----- process event  --> step_done
-- on exit: Busy --> Idle
+stopMotor
-- on entry: Busy --> Idle
----- process event  --> step_done
----- process event  --> disable
-- on exit: On --> Off
+disableMotor
-- on entry: On --> Off
```
