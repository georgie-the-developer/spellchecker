## Building the Program

### **Linux**

1. Clone the repository.
2. Open a terminal and navigate to the project root (the folder should be named `spellchecker`).
3. Run:

   ```sh
   ./scripts/build.sh
   ```
4. The compiled program will appear in `./bin/` as `main`.

---

#### **windows**

1. Open **Visual Studio**.
2. Choose **Clone Repository**.
3. Enter:

   ```
   https://github.com/georgie-the-developer/spellchecker.git
   ```
4. Click **Clone**.
5. Open the **Developer Terminal** inside Visual Studio.
6. Navigate to the project root (`spellchecker`).
7. Run:

   ```bat
   .\scripts\build.bat
   ```
8. The compiled program will appear in `.\bin\` as `main.exe`.

---

## Running the Program

From the repository’s root directory:

* **Linux:**

  ```sh
  ./bin/main
  ```

* windows:

  ```bat
  .\bin\main.exe
  ```

The program accepts **one optional command-line argument**: a filepath to a custom dictionary.
If more than one argument is provided, the program behaves as if none were given.
If no argument is passed, it loads the default dictionary.

---


