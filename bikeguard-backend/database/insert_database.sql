CREATE DATABASE UserDB;
GO

USE UserDB;
GO

CREATE TABLE users (
    id INT IDENTITY(1,1) PRIMARY KEY,
    first_name NVARCHAR(100),
    last_name NVARCHAR(100),
    email NVARCHAR(255) UNIQUE,
    phone NVARCHAR(20),
    password NVARCHAR(255)
);