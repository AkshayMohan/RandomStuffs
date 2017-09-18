# all2HTML

Licensed under MIT.

This program changes the extension all files from a path mentioned to ".html" extension. It uses SQLite to keep track of the changes so that everything can be undone. This program is done for someone.

**Changelogs**

v1.0.3 - 18th of September, 2017.

- Fixed duplicate data entry. Delete the previous "all2HTML.db" and start the program again to use the latest table structure. (Ensure to rollback any changes so as to avoid any conflicts.)

v1.0.2 - 15th of September, 2017.

- Added command line arguments for the program. Run it as "all2HTML.exe --help" for instructions.
- Fixed : Folder names getting appended with ".html" extension when subfolders are not included for conversion.

v1.0.1 - 4th of September, 2017.

- Changed "convertFileTypeToHTML" function to "changePathExtension" with additional two arguments - the extension to be set and the number of files in which changes have occurred.
- Code cleanup and other memory leaks have been fixed!
- Conversion function has been drastically improved.

v1.0.0 - 27th of August, 2017.

- Initial release.
