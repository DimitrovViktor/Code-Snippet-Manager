# A simple C++ Code Snippet Manager (CSM)

A lightweight CLI app for storing, searching, editing, and deleting programming code snippets.
CSM uses SQLite and has a simple interactive menu for managing snippet data.

---

![App](https://github.com/user-attachments/assets/6af2b0ed-2d3e-45a5-8725-c277ec5b285d)

---

## Overview:

The Code Snippet Manager is designed to help with organizing small pieces of code by language and tags.

Each snippet entry contains:
 - Code
 - Programming language
 - Tag(s)
 - Automatically assigned ID

## Features:

### Menu

The menu has four options:

 - Add Snippet
 - Search Snippet
 - Delete Snippet
 - Edit Snippet

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
 - ID Search - serach snippets by ID
 - Show all snippets

### Delete Snippet

Asks for ID and removes snippet from db.

### Edit Snippet

Asks for ID and allows users to:

 - Replace Code
 - Replace Tags
 - Replace Language

## To-do:

 - Add GUI/TUI
