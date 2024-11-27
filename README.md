# C_Base

A Pocketbase alternative writen in C and using [Mongoose web server](https://mongoose.ws/) and [SQLite](https://www.sqlite.org/)

**This is probably never going to be a production grade application**

## Main Goals :

- Less than 5MB executable
- Embbeded UI 

## TODO : 

- [X] Serve public folder beside admin UI (need to ask mongoose dev)
- [X] Create default directories if not already existing (same for DB file)
- [X] Make SQL editor + display result
- [X] Create default tables in DB if not already existing (ADMIN & USERS & LOGS)
- [X] Display created tables + rows (not admin nor log tables!)
- [ ] Auth check on request (admin & user for api)
- [ ] JWT Auth
- [ ] Hash + check user password
- [ ] Add mongoose errors + input req + sqlite errors in the log table