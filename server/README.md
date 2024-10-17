# Backend Server for Kuserfeedback

### Usage

inside the `server` folder, run the following command:

```bash
docker-compose up --build -d
```

This will run the service in the background. At the moment, it's just a Proof of
Concept, so passwords and sensitive data is hardcoded.

### Documentation

This docker-compose is made of two parts: an Apache server and a mysql database.

The Apache server is a lightweight and basic image, to keep everything as slim
and simple as possible. The database is mysql and not sqlite due to requirements
given by Banana.ch

### Errors

The backend server is supposed to support 3 different DBMS, but actually only
one of them (sqlite) is 100% covered. This led to a set of changes to the
original project in order to fully adapt it to mysql. Even with a lot of work
and effort, there may be some incongruencies, that you are kindly invited to
report to the developers. Thank you.
