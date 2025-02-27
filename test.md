Test case 1:
Average case
Input: 
    Rod Length = 12
    Piece value list: 2, 3 4, 5 5, 6
Output:
    Rod Length: 12
    6 @ 2 = 18
    Total Value: 18
    Remainder: 0

Test Case 2:
Remainder Case:(one piece)
Input: 
    Rod Length = 10
    Piece value list: 8, 4
Output:
    Rod Length: 10
    1 @ 8 = 4
    Total Value: 4
    Remainder: 2

Test Case 3:
Remainder Case:(mult piece)
Input: 
    Rod Length = 20
    Piece value list: 7, 3 6, 4
Output:
    Rod Length: 20
    3 @ 6 = 12
    Total Value: 12
    Remainder: 2

Test Case 4:
Not Value-based
Input:
    Rod Length = 8
    Piece value List: 6, 5 4, 4
Output:
    Rod Length: 8
    2 @ 4 = 8
    Total Value: 8
    Remainder: 0

Test Case 5:
No dupes
Input:
    Rod Length = 8
    Piece value list: 5, 4 3, 2
Output:
    Rod Length: 8
    1 @ 5 = 4
    1 @ 3 = 2
    Total Value: 6
    Remainder: 0