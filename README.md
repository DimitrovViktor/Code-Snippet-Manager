# <div align="center"> A simple C++ Code Snippet Manager (CSM) </div>

<div align="center">

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)
[![SQLite](https://img.shields.io/badge/SQLite-%2307405e.svg?logo=sqlite&logoColor=white)](#)
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)

A lightweight CLI app for storing, searching, editing, and deleting programming code snippets.
CSM uses SQLite and has a simple interactive menu for managing snippet data.

</div>

---

![App](https://github.com/user-attachments/assets/6af2b0ed-2d3e-45a5-8725-c277ec5b285d)

---

## Overview

The Code Snippet Manager is designed to help with organizing small pieces of code by language and tags.

Each snippet entry contains:
 - Code
 - Programming language
 - Tag(s)
 - Automatically assigned ID

## Features

### Menu

The menu has four options:

 - Add Snippet
 - Search Snippet
 - Delete Snippet
 - Edit Snippet

![Program](https://github.com/user-attachments/assets/6f9beb4f-3e1c-4904-ae05-b17c986f2d10)

### Add Snippet

Snippets are appended directly to the database.

Snippet adding steps:

 - Code adding - choose code of snippet
 - Language adding - choose language of snippet
 - Tag adding - choose tag of snippet
   
### Search Snippet

Search modes:

 - Language Search - search snippets by language
 - Tag Search - search snippets by tag
 - ID Search - search snippets by ID
 - Show all snippets

![SearchSnippet](https://github.com/user-attachments/assets/5b350e57-c12b-4f32-8165-f2e49cc8789f)

### Delete Snippet

Asks for ID and removes snippet from db.

### Edit Snippet

Asks for ID and allows users to:

 - Replace Code
 - Replace Tags
 - Replace Language

![EditSnippet](https://github.com/user-attachments/assets/469739d2-e645-44fa-ab84-a9e396c4f25d)

## Qt GUI Version

The Qt6 desktop application is in the `Qt/CSM/` directory. It provides a search bar with filtering by language, tag, or code, a snippet list with preview, and buttons for adding, editing, deleting, and copying snippets to clipboard.

## Dependencies

 - SQLite - embedded database engine
 - Qt6 - GUI framework

## Setup

1. Download SQLite amalgamation
2. Place `sqlite3.c` and `sqlite3.h` inside `Qt/CSM/sqlite/`
`next steps are for Qt (GUI)`
3. Open `Qt/CSM/CMakeLists.txt` in Qt Creator
4. Select a Desktop kit and build
