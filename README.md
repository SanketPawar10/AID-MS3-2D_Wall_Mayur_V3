WARNING: Uninstall 'Qt Visual Studio Tools' extension and 'Qt Vs Cmake Tools' extension from Visual Studio before opening the project.

REQUIREMENTS: 
1. Remove Qt path from your system variable if you have already installed it before.
2. Download and place Qt 6.6.0 libraries inside thirdparty/qt6 folder.

Steps to Download Qt 6.6.0 using aqtinstall python tool

Requirement: Internet access, Python, pip
1. Open cmd. 
2. Type "python". Ignore the quotes. If Python is not installed, microsoft store will open itself, install it. If previously installed, type "quit()" and press enter to get out of the python interpreter and skip step 3 below.
3. Again type python to see if it is installed, close cmd if everything's good.
4. Reopen cmd and type "pip install -U pip". If it says already installed, good.
5. Type "pip install aqtinstall"
6. Type "mkdir c:\Qt", if it is not working, just create a folder called "Qt" in C:\. [NOTE: You can use any folder, even install directly to thirdparty/qt6/ folder inside the project repo.]
7. Now type "python -m aqt install-qt --outputdir c:\Qt windows desktop 6.6.0 win64_msvc2019_64 -m qt3d". It will take sometime.
8. If everything went well, you should have Qt installed.
9. Now open the C:\Qt\6.6.0 folder in windows explorer, copy the \msvc2019_64\ folder, paste it in thirdparty/qt6/ of the project folder. The folder name should be "msvc2019_64"
10. The folder structure should be like project_folder\thirdparty\qt6\msvc2019_64\. For example, my folder structure for bin folder is D:\Project Share Folder\Application\AID_MS3\thirdparty\qt6\msvc2019_64\bin.

Visit https://aqtinstall.readthedocs.io/en/latest/installation.html for a detailed step-by-step instructions if needed.
