
/*

-- Details for 0.75

STILL TO DO:

1.  Picture buttons.
2.  Proper Slideshow list

DONE:


-- Details for 0.72

DONE:

1.  Added option to allow a fullscreen start
2.  Fixed the slideshow feature
3.  Fixed the fullscreen. It's nice now
4.  Added an extra viewing mode: PEEK_IMAGE_SCALED_NICELY. It is good.
5.  Fixed minor bugs lying around.
6.  Fixed major bugs lying around.
7.  The DeleteFile now clobbers any files already in the trash with the 
    same name.
8.  Removed the ThumbnailMode. Peek was trying to do too much. 
9.  Removed the plugin capabilities - no one seemed to use them. Sorry if 
    anyone did.
10. Changed the settings file to a text-based format. It uses my class
    called ConfigureClass. NOTE: This can crash Tracker if the settings
    directory is open while Peek is shutdown. This is because of the method
    of re-writing a file each time a setting is changed. Too many messages
    for the poor Tracker.
11. Fixed the clipping bugs.
12. Removed the slideshow/normal "load into center" setting cross-bleed.
    The option only affects the slideshow now, as it should.
13. Made sure that Peek detects any old setting files and deletes them
    before creating the new one.
14. Found a silly bug in the SimpleData handler for drag & drop. I don't know
    how it got there and I don't know how long it's been there.
15. Added the support for read-only volumes (ie CD-ROM). This allows Peek
    to view images on a CD-ROM which didn't work because they could not
    have mime-types. My IsType method always returned false.

*/