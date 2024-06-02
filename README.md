# klex
klex is a text editor written in C using ncurses library as a learning project

## Quick Start

"Dependecies : gcc,ncurses,make"


1.Go to source directory

```sh
 cd path/to/klex/src

 ```
2.Compile the source code

 ```sh
 make

 ```
3.Run the editor

 ```sh
 ./klex
```

### Keybindings
                           
|   Mode    |  Keybind  |           Description            |
|-----------|-----------|----------------------------------|
| Global    | Ctrl + Q  |   Quit,any mode                  |
| Normal    | h         |   Move left                      |
| Normal    | j         |   Move down                      |
| Normal    | k         |   Move up                        |
| Normal    | l         |   Move right                     |
| Normal    | x         |   Delete character               |
| Normal    | d         |   Delete entire line             |
| Normal    | i         |   Enter insert mode              |
| Normal    | g         |   Go to first line               |
| Normal    | G         |   Go to last line                |
| Normal    | 0         |   Go to beginning of the line    |
| Normal    | $         |   Go to end of the line          |
| Normal    | w         |   Go to next word                |
| Normal    | b         |   Go to previous word            |
| Normal    | e         |   Go to end of the word          |
| Normal    | Ctrl + S  |   Save                           |        
| Insert    | Esc       |   Exit insert mode               |        


