Name:
Apartments for sale

Description:
This project was part of my college course "Advanced Programming in C" at my second year.
In this project i created a database of apartments for sale to use by a real estate agent.

Usage:
The program accepts the following commands:
exit: closes the program.
find-apt *: all apartments that fullfil the * constraint will be printed to screen. at the end of the find-apt command adding -s/-sr will print the expected data in ascending/descending order by price. 
add-apt "Street #Building City" Price #Rooms Date: adds the apartment to the databse.
buy-apt #Code: The apartment with the code given is marked "sold" and is excluded from the database.
delete-apt -Enter #Days: Deletes from the database the apartments that entered in the last #Days mentioned.
!!: executes the most recent command
short_history: prints the 7 latest commands
hisroty: prints all the commands
!<num>^str1^str2: executes the command that has the code <num> which contains substring str1 and switches it with substring str2

Constraints for find:
MaxPrice
MinPrice
MinNumRooms
MaxNumRooms
Date
Enter

Examples:
find-apt –MinNumRooms 3 –MaxNumRooms 5 –MaxPrice 1750000 (it is possible to combine few constraints but if the order matters add the order command at the end of the line)
add-apt "Hasavyonim 23 Ramat Ono" 2000000 4 01 01 21
buy-apt 6
delete-apt –Enter 9
!4^3^2

