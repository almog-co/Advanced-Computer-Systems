# Final Project - Repeatable Read and Read/Write Locking for a Database and Comparing the Performance to Sequential Transactions
By: Almog Cohen and Rojan Karn

The goal of this project is to create a database system that allows for basic commands and transactions with repeatable read. Each transaction will support repeatable read and read/write locking to ensure transaction isolation. Each transaction will run independently on a separate thread pool and SIMD instructions will be used to search through the database efficiently. Transactions will support read/write operations, if/else statements, and select statements.
