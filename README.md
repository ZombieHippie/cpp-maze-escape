# cpp-maze-escape
escaping a maze in cpp, ex for std::fin tokenization

Solves mazes which come in the form:
```
<Maze width>
<Maze height>
<Number of barrier locations>
<Barrier location: "<x> <y>">***
```

For example, this [SmallMaze](https://github.com/colelawrence/cpp-maze-escape/blob/master/ConsoleApplication1/SmallMaze.txt)
is 6x6 and has 7 seven barriers. One of which is at (0, 2)
```
6
6
7	
0 2
1 2
3 0
3 1
3 5
5 3
0 4
```

So, this maze might look like the following:
```
........
|S  B  |
|   B  |
|BB    |
|      |
|B    B|
|   B F|
........
```
[S]tart, [F]inish
