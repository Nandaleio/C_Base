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
- `cb_data`: store the SQLite DB, files and logs
- `cb_public`: serve static files _(e.g.: the website that will consume the API)_
- `cb_hooks`: contains the custom JS logics **_(not implemented yet)_**

The default port is `0.0.0.0:8080` but you can change it with the `--host` or `-p` program argument\
You can then access :
- `localhost:8080/`: `cb_public` content
- `localhost:8080/_/`: admin UI
- `localhost:8080/api`: API
- `localhost:8080/api/amdin`: admin API


**Windows and Linux are currently the only supported platforms !** 

C_Base **does not** and **will not** provide any SDK.\
But you can query the API through any HTTP client.

## Roadmap :

The final goal is to implements as many features Pocketbase offers.

- [X] Single executable
- [X] SQLite through HTTP _(Basic CRUD)_
- [X] Authentication _(only SHA256 encryption for now)_
- [ ] API Rules
- [ ] Real time change through websocket
- [ ] JS engine to add custom behavior _(probably gonna use [Duktape Engine](https://duktape.org/))_


## Why C_Base :

C_Base is just a little personal challenge so I can learn a bit of C _(that's why it's probably full of memory leaks)_.

Feel free to open an issue or start a discussion if you have any questions or improvement suggestions

## TODO : 

Next items I will work on :

- [ ] Create table from Admin UI 
- [ ] Where clause from API
- [ ] Prevent access to password and salt from user table _(didn't find elegant way to do it yet)_
