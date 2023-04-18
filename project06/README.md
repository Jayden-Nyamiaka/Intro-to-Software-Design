In addition to completing the project, your group will need to maintain a log of who does what throughout the project. The log should be written in **this README** and updated consistently to reflect the group's progression. For more details about what this log should contain, please read the following document https://sof.tware.design/22sp/documents/project-logs.

This project log is **mandatory** - if you do not have a complete and proper project log, **you will not be able to have in-person office hours or code reviews with your TAs**.

# Log Entries
## (Sample) Wednesday, 3/23, 9:30 - 11:30pm
**WHO:** Sarah and Alice

**WHAT:** Working on wc.c, particularly reading in characters from a file

**BUGS:** We kept reading past the end of the file, so our while loop would run forever. We fixed this by reading the documentation for fread to figure out how to check the return condition correctly, and changing our code accordingly

**RESOURCES USED:** https://sof.tware.design/22sp/docs/fread


Who: Jayden, Dilichi, Daniel
When: Thursday 5/12, 6:00pm - 12:00pm
What: started implementing collision resolution returning axis in find_collision, 
    implemented impulse algorithm in physics collision handler,
    fixed impulse algorithm to support INFINITY mass bodies,
    added removable field to body with supporting functions to enable single-body removal collisions,
    made significant restructure in forces interface to support new collisions,
    updated forces to incorporate collision_handler_t functions,
    made generic apply_collision forcer utilizing collision_handler
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: David, Jayden, Dilichi
When: Friday 5/13, 2:00pm - 9:00pm
What: started breakout demo,
    added all objects to the screen, made thick walls,
    added destructive and physics collisions from new interface,
    encountered bug where demo randomly stops due to memory access error,
    fixed bug by going over and changing scene tick,
    made it so paddle gets faster with every brick destroyed (> 1 elasticity),
    finished breakout demo but still no extra feature
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: David, Daniel, Dilichi
When: Saturday 5/14, 10:00am - 8:00pm
What: started adding and researching audio but decided against it due to time constraints,
    added support for vector and body dilations,
    used body dilations to lengthen paddle every 5 bricks,
    encountered bug where paddle lengthens extensively to no end,
    fixed paddle lengthening bug by utlilizing an object_count in state,
    made reset and quit options for player using R and Q keys respectively,
    encountered bug where pegs does not work because of differences in our implementation and pegs implementation
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: Jayden, David, Daniel, Dilichi
When: Sunday 5/5, 2:00pm - 6:00pm
What: added collision_aux to support handler that only needs a constant,
    fixed our implementation of forces to match pegs implementation so now pegs works fixing previous bug,
    updated sdl_wrapper to support more keys,
    added ability for paddle to inverse position on screen using X key,
    changed ability for paddle to teleport to middle from space key to Z key,
    added instructions
Resources Used: https://sof.tware.design/22sp/docs/fread
