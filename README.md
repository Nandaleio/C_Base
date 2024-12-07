# C_Base

A [Pocketbase](https://pocketbase.io/) alternative _(or copy)_ written in C and using [Mongoose web server](https://mongoose.ws/) and [SQLite](https://www.sqlite.org/)

**_!!! This is probably never going to be a production grade application !!!_**

## Main Features :

- SQLite DB
- Authentication
- Embedded admin UI
- Less than 5MB single executable _(might exceed a bit in the future)_

## How to use:

Just run the C_Base.exe.\
C_Base will create several folders :
- **cb_data**: store the SQLite DB, files and logs
- **cb_public**: serve static files _(eg: the web site that will consume the API)_
- **cb_hooks**: contains the custom JS behaviour **_(not implemented yet)_**

**Windows is currently the only supported platforms !** 

## Roadmap :

- [ ] Real time change through websocket
- [ ] JS engine to add custom behavior _(probably gonna use [QuickJS](https://bellard.org/quickjs/) or [Duktape Engine](https://duktape.org/))_

## TODO : 

- [ ] Prevent access to password and salt from user table
- [ ] Create table from Admin UI 
- [ ] Where clause from API
- [ ] API Rules