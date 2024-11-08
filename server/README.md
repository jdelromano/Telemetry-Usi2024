# Backend Server for Kuserfeedback

### Usage

First, copy the file `example.env` to `.env` using the following command:

```bash
cp example.env .env
```

then edit the file adding your custom entries.

inside the `server` folder, run the following command:

```bash
docker-compose up --build -d
```

This will run the service in the background.

### Documentation

This docker-compose is made of three parts: an Apache server, a mysql database
and a Grafana dashboard.

The Apache server is a lightweight and basic image, to keep everything as slim
and simple as possible. The database is mysql and not sqlite due to requirements
given by Banana.ch Grafana is a powerful tool to visualize data, and it is used
to display the data stored in the database.

### First steps

Everything will start automatically, but you need to login to Grafana with the
default credentials:

- user: admin
- password: admin then you will be asked to change the password. after that, you
  need to add a new data source, with the following parameters:
- type: mysql
- host: db (this is the name of the mysql container on docker)
- database name: user_feedback
- username: grafanaReader
- password: grafanaReader (this is the default in example.env)

### Errors

The backend server is supposed to support 3 different DBMS, but actually only
one of them (sqlite) is 100% covered. This led to a set of changes to the
original project in order to fully adapt it to mysql. Even with a lot of work
and effort, there may be some incongruencies, that you are kindly invited to
report to the developers. Thank you.
