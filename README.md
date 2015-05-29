# CallBehavior
Call Behavior Node For Unreal Engine 4

Description and example project: https://forums.unrealengine.com/showthread.php?71407-Call-Behavior-Node-Idea-and-Code


Adding to your project:

1. Copy Call behavior source files into Source folder of your project
2. In VS add those files to your project
3. Modify #includes in .cpp files to have your project .h
4. In YourProject.Build.cs file and add AIModule at line 9

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule" });

5. If you want logs define your log category in Your project.h and .cpp and replace LogMyProject by this, otherwise just comment all UE_LOG in .cpp files

6. Call behavior can triger "duplicate keys" warnings while running. You may want to disable it by adding this in your DefaultEngine.ini :

[Core.Log]
LogBehaviorTree=Error

7. Build and run.



Using:

1. In order to use CallBehavior you have to specify MyController as AIController class in your bot.

2. To create a behavior function, which you can then invoke in Call Behavior node, the following is required:

2.1 Create a new Behavior tree (we'll call it BF as Behavior Function) and a corresponding blackbord (BB)
2.2 The top level node in BF must have PopBB Service. This service will call PopBlackboard method in MyBlackboard component on deactivation.
2.3 If a blackboard key is a parameter append "PARAM_" to its name.

3. To call a behavior:

3.1 Create a CallBehavior node in your behavior tree, specify your behavior function in Behavior Asset field
3.2 Specify arguments you want to pass in Params array. They should appear in exactly the same order that the parameters in the behavior function blackbord. There is no checks.
