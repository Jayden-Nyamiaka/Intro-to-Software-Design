In addition to completing the project, your group will need to maintain a log of who does what throughout the project. The log should be written in **this README** and updated consistently to reflect the group's progression. For more details about what this log should contain, please read the following document https://sof.tware.design/22sp/documents/project-logs.

This project log is **mandatory** - if you do not have a complete and proper project log, **you will not be able to have in-person office hours or code reviews with your TAs**.

# Log Entries
## (Sample) Wednesday, 3/23, 9:30 - 11:30pm
**WHO:** Sarah and Alice

**WHAT:** Working on wc.c, particularly reading in characters from a file

**BUGS:** We kept reading past the end of the file, so our while loop would run forever. We fixed this by reading the documentation for fread to figure out how to check the return condition correctly, and changing our code accordingly

**RESOURCES USED:** https://sof.tware.design/22sp/docs/fread

Who: Jayden, Dilichi
When: Tuesday 5/3, 5:00pm - 1:00am
What: Worked on and finished collision.c, 
    implemented scene_add_bodies_force_creator in replace of scene_add_force_creator,
    implemented create_destructive_collision, fixed small bug in list_free,
    updated body functions, started getting demos working with new interface,
    created new struct for force_creators to be held in scene replacing old one,
    encountered bug in gravity demo where it throws an exception right away
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: David, Jayden, Dilichi, Daniel
When: Wednesday 5/4, 12:00pm - 2:00am
What: Started and finished implemting new space_invaders demo, 
    got all demos working with new interface, deprecated all unused code,
    adjusted key controls for player in space_invaders demo
    updated scene_tick to use new force_implementation, added win and lose mini screens,
    added info_t struct for void *info in body, caught bugs in new force implementation, 
    encountered bug in collision.c where collisions only work sometimes 
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: David, Dilichi, Daniel
When: Thursday 5/5, 1:30pm - 4:00pm
What: Worked on fixing collision algorithm,
    basic debugging and design preference changes in space invaders demo,
    made player laser rely on if laser exists rather than reload speed
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: David, Jayden, Daniel
When: Thursday 5/5, 7:00pm - 10:00pm
What: Fixed collision algorithm, caught many related bugs in scene_tick,
    added blockades as nice finishing touch, solidified final constants,
    added more math support in vector that is used in the collision algorithm,
    added collisions btwn bodies other than just lasers (like btwn players and enemies)
Resources Used: https://sof.tware.design/22sp/docs/fread
