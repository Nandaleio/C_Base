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
    - cb_data: store the SQLite data file + files
    - cb_public: serve static files (eg: the web site that will consume the API)

**Windows is currently the only supported platform** 

## TODO : 

- [X] Create default tables in DB if not already existing (ADMIN & USERS & LOGS)
- [X] Display created tables + rows (not admin nor log tables!)
- [X] Auth check on request (admin & user for api)
- [X] JWT Auth
- [X] Hash + check user password
- [ ] JWT check: expiration date
- [ ] Add mongoose errors + input req + sqlite errors in the log table