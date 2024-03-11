Download Qt 6.6 or above

Requirement: Internet access, Python, pip

Install Qt 6.6.0 for Microsoft Windows Visual Studio 64-Bit
1. Open cmd. 
2. Type "python". Ignore the quotes. If Python is not installed, microsoft store will open itself, install it. If installed, type "quit()" and press enter to get out of the python interpreter and skip step 3 below.
3. Again type python to see if it is installed, close cmd if everything's good.
4. Reopen cmd and type "pip install -U pip". If it says already installed, good.
5. Type "pip install aqtinstall"
6. Type "mkdir c:\Qt", if it is not working, just create a folder called "Qt" in C:\.
[Optional] To see available versions, type: "python -m aqt list-qt windows desktop"
[optional] To see available architecture for the chosen version, type "aqt list-qt windows desktop --arch 6.6.0". 
7. Now type "python -m aqt install-qt --outputdir c:\Qt windows desktop 6.6.0 win64_msvc2019_64". It will take sometime.
8. If everything went well, you should have Qt installed.
9. Now open the C:\Qt folder in windows explorer, copy the msvc folder, paste it in thirdparty/qt6/ of the project folder. The folder name should be "msvc2019_64"


Visit https://aqtinstall.readthedocs.io/en/latest/installation.html for a detailed step-by-step instructions.