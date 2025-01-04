# C_Base

A [Pocketbase](https://pocketbase.io/) alternative _(or copy)_ written in C and using [Mongoose web server](https://mongoose.ws/) and [SQLite](https://www.sqlite.org/)

**_!!! This is probably never going to be a production grade application !!!_**\
**_!!! Expect crash, segfault and many other problems while using the app  !!!_**

## Main Features :

- SQLite DB
- Authentication
- Embedded admin UI
- Less than 5MB single executable _(might exceed a bit in the future)_

**Windows and Linux are currently the only supported platforms !** 

C_Base **does not** and **will not** provide any SDK.\
But you can query the API through any HTTP client.

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

The default admins credentials are _username: **admin** password: **admin**_\
Change it as soon as you log into the app for the first time.


## API Description 

### Standard API

| URL |HTTP Verb| Description | Expected Body
|-|:-:|-|:-:|
|`/api/auth/login"`| POST | User login | {username:string, password:string} |
|`/api/auth/register`| POST | User registration | {username:string, password:string} |
|`/api/tables`| GET | List available tables except the C_Base ones ||
|`/api/table/:table?where=...`| GET | Get the table rows according to the optional where clause ||
|`/api/version`|  GET |Return the current version of the C_Base instance ||


### Admin API
| URL |HTTP Verb| Description | Expected Body
|-|:-:|-|:-:|
|`/api/admin/login`| POST |Admin login | {username:string, password:string}|
|`/api/admin/logs`| GET |Get the C_Base instace logs||
|`/api/admin/query`| POST | Query the database| {query: string}|
|`/api/admin/admins`| GET | List all the existing admins ||
|`/api/admin/admin`| POST | Create a new admin| {username:string, password:string}|
|`/api/admin/admin/:adminId`| DELETE | Delete the admin by id ||
|`/api/admin/table/:tableName`| POST | Create a new table ||
|`/api/admin/table/:tableName`| DELETE | Delete the table by _tableName_ ||
|`/api/admin/configs`| GET | Get all the C_Base configuration ||
|`/api/admin/configs`| PUT | Set the C_Base configuration | {name:string, value:string} |


## Roadmap :

The final goal is to implements as many features Pocketbase offers.

- [X] Single executable
- [X] SQLite through HTTP _(Basic CRUD)_
- [X] Authentication _(only SHA256 encryption for now)_
- [ ] API Rules
- [ ] Real time change through websocket
- [ ] JS engine to add custom behavior _(probably gonna use [Duktape Engine](https://duktape.org/))_
- [ ] Mail


## Why C_Base :

C_Base is just a little personal challenge so I can learn a bit of C _(that's why it's probably full of memory leaks)_.

Feel free to open an issue or start a discussion if you have any questions or improvement suggestions

## TODO : 

Next items I will work on :

- [ ] Create table from Admin UI 
- [ ] Config table for Admins
- [ ] Where clause from API
- [ ] Prevent access to password and salt from user table _(didn't find elegant way to do it yet)_
