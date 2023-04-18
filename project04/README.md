In addition to completing the project, your group will need to maintain a log of who does what throughout the project. The log should be written in **this README** and updated consistently to reflect the group's progression. For more details about what this log should contain, please read the following document https://sof.tware.design/22sp/documents/project-logs.

This project log is **mandatory** - if you do not have a complete and proper project log, **you will not be able to have in-person office hours or code reviews with your TAs**.

# Log Entries
## (Sample) Wednesday, 3/23, 9:30 - 11:30pm
**WHO:** Sarah and Alice

**WHAT:** Working on wc.c, particularly reading in characters from a file

**BUGS:** We kept reading past the end of the file, so our while loop would run forever. We fixed this by reading the documentation for fread to figure out how to check the return condition correctly, and changing our code accordingly

**RESOURCES USED:** https://sof.tware.design/22sp/docs/fread

Who: Jayden, Dilichi
When: Wednesday 4/27, 7:30am - 1:30pm
What: Ported everything over, started on and made significant progress on demos, 
        made gravity force creator, implemented force_creator_t in scene
Bugs: Makefile not working (fixed midway - removed color, added random to STUDENT_LIBS)
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: David, Dilichi, Jayden, Daniel
When: Wednesday 4/27, 2:00pm - 2:00am
What: Working on forces.c for all functions, finished both new demos, 
        finished implementing forces, made updates to scene and body,
        started test files
Bugs: Gravity needs to be fixed with new interface
Resources Used: https://sof.tware.design/22sp/projects/04

Who: Dilichi, Jayden
When: Thursday 4/28, 11:30am - 1:15pm
What: Fixed gravity to work with new interface, caught some minor bugs in body,
        added more support (get and set) functions for body, finalized demos,
        added custom create_earth_gravity for gravity demo in forces 
Resources Used: https://sof.tware.design/22sp/projects/04

Who: David, Daniel, Jayden
When: Thursday 4/28, 7:30am - 8:30pm
What: Checked force tests passed on our implementation, ported over revised pacman,
        caught minor bug in body_tick (set velocity to average of before and after tick)
Resources Used: https://sof.tware.design/22sp/projects/04

Who: David, Jayden
When: Sunday 5/1, 1:30pm - 2:45pm
What: Implemented revisions: used list of bodies in aux to remove passing NULL,
        removed stored dt, made force_holder_t as type for free func instead of void
Resources Used: https://sof.tware.design/22sp/projects/04
