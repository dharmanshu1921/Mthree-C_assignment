# 💸 SmartExpense - Personal Finance Manager (C Project)

**Author:** Dharmanshu Singh  
**Language:** C  
**Platform:** GCC (Linux/Mac)  
**Purpose:** End-to-end personal finance management tool in C, demonstrating mastery of C programming concepts.

---

## Overview

SmartExpense is a **command-line personal finance manager** built entirely in C.  
It allows users to **track income and expenses**, set a **monthly budget**, view **summaries**, and **export data to CSV**.  

This project demonstrates practical usage of C programming concepts including **structs, dynamic memory management, file I/O, enums, time handling, modular programming, and string manipulation**.

---

## Features

1. **User-friendly CLI with colorized output** using ANSI escape codes.
2. **Add transactions** with:
   - Type: Income / Expense
   - Amount in INR
   - Category
   - Short description
3. **List transactions** in **chronological order** with IDs for easy reference.
4. **Delete transactions** using their ID.
5. **Summary & Analytics**:
   - Total income and expenses
   - Net balance
   - Average expense per transaction
   - Category-wise breakdown
6. **Budget Management**:
   - Set monthly budget
   - Check if budget is exceeded
7. **Data persistence**:
   - Transactions are saved in binary files specific to each user
   - Export transactions to CSV for external analysis
8. **Personalized greeting** for the user at program start.

---

## Project Structure

```text
Mthree_Prj/
├─ main.c             # Main program with CLI
├─ lib/
│  ├─ pexpense.c      # Library with transaction & budget management logic
│  └─ pexpense.h      # Header file for the library
├─ Result/            # Screenshots of program execution
│  ├─ 1.png
│  ├─ 2.png
│  ├─ 3.png
│  ├─ 4.png
│  ├─ 5.png
│  ├─ 6.png
│  └─ 7.png
└─ Makefile           # Compilation instructions
```



---

## C Programming Concepts Used

- **Structs:** `Transaction`, `TxList`, `Budget`, `CategorySummary`  
- **Dynamic memory management:** `malloc`, `realloc`, `free`  
- **Enums:** `TxType` for income/expense  
- **File I/O:** Binary file read/write for persistence; CSV export  
- **Time handling:** `time_t`, `struct tm`, `strftime` for timestamps  
- **Strings:** Safe input handling with `fgets` and `strcspn`  
- **Functions & modularity:** All main operations are encapsulated in separate functions  
- **Error handling:** Validates file operations, input, and IDs  

This demonstrates **mastery of fundamental and intermediate C programming concepts**, as well as real-world problem solving.

---

## Screenshots

All results of program execution are shown below (images are in the [`Result`](https://github.com/dharmanshu1921/Mthree-C_assignment/tree/main/Result) folder):

![1](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/1.png)

![2](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/2.png)

![3](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/3.png)

![4](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/4.png)

![5](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/5.png)

![6](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/6.png)

![7](https://github.com/dharmanshu1921/Mthree-C_assignment/raw/main/Result/7.png)

---

## How to Compile & Run

```bash
# Clone the repository
git clone https://github.com/dharmanshu1921/Mthree-C_assignment.git
cd Mthree-C_assignment

# Compile using Makefile
make

# Run the program
./smartexpense 
