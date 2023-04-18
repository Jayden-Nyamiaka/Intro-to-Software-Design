In addition to completing the project, your group will need to maintain a log of who does what throughout the project. The log should be written in **this README** and updated consistently to reflect the group's progression. For more details about what this log should contain, please read the following document https://sof.tware.design/22sp/documents/project-logs.

This project log is **mandatory** - if you do not have a complete and proper project log, **you will not be able to have in-person office hours or code reviews with your TAs**.

# Log Entries
## (Sample) Wednesday, 3/23, 9:30 - 11:30pm
**WHO:** Sarah and Alice

**WHAT:** Working on wc.c, particularly reading in characters from a file

**BUGS:** We kept reading past the end of the file, so our while loop would run forever. We fixed this by reading the documentation for fread to figure out how to check the return condition correctly, and changing our code accordingly

**RESOURCES USED:** https://sof.tware.design/22sp/docs/fread

Who: Daniel, Dilichi
When: Wednesday 4/20 6:30pm - 9:00pm
What: Worked on creating body.c, finished body.c, worked on and finished scene.c
Resources Used: https://sof.tware.design/22sp/docs/fread

Who: Daniel, David
When: Thursday 4/21 1:00pm - 3:00 pm
What: Imported vector and polygon, made/fixed list, edited body and scene to fit list
Resources: https://sof.tware.design/22sp/docs/fread

Who: Jayden, Dilichi, Daniel
When: Thursday 3:00pm - 9:00pm
What: Ported over and fixed previous demos and fix, worked on pacman, made random .h and .c files
Resources: https://sof.tware.design/22sp/docs/fread

Who: Daniel, Dilichi
When: Thursday 10:00pm - Thursday 11:30pm
What: Continued modifying previous demos to work with new interface, added helper
 functions and fields for more support
Resources: https://sof.tware.design/22sp/docs/fread

Who: Jayden, Daniel
When: Friday 1:30am - 4:00am
What: Finished up pacman, fixed overlapping includes, adapted Makefile, debugging
        finished adapting demos to interfaces 
        Ran into substantial bug trying to figure out how to utilize sdl_on_key
Resources: https://sof.tware.design/22sp/docs/fread

Who: Jayden, Daniel, Dilichi
When: Friday 11:30am - 8:30pm
What: Significant debugging, figured out and utilized on_key key handler, fixed list_free, edited 
        Makefile, many small changes to helper functions, fixed includes, completed pacman, added 
        small tweaks for functionality problems, ensured spec is met with encapsulation
Resources: https://sof.tware.design/22sp/docs/fread

Who: David, Jayden, Dilichi
When: Thursday 4/28, 3:00pm - 4:00pm
What: Revisions for the PLUS:
        Added background and pacman to state, fixed index specific design errors related to this
        Fixed code duplication in on_key / key_handler method with helper function
        Made pacman_get_center method to further fix code duplication
        Removed magic numbers for orientation as #define doubles
Resources: https://sof.tware.design/22sp/docs/fread