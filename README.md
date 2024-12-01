# C_Base

A Pocketbase alternative writen in C and using [Mongoose web server](https://mongoose.ws/) and [SQLite](https://www.sqlite.org/)

**This is probably never going to be a production grade application**

## Main Features :

- SQLite DB
- Authentication
- Embbeded admin UI
- Less than 5MB executable

## How to use:

Just run the C_Base.exe.\
C_Base will create several folder :
- cb_data: store the SQLite DB, files and logs
- cb_public: serve static files (eg: the web site that will consume the API)

**Windows and linux is currently the only supported platforms** 

## TODO : 

- [ ] JWT check: expiration date
- [ ] Add mongoose errors + input req + sqlite errors in the log table