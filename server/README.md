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

Everything will deploy automatically, you just have to login to grafana with the
following credentials:

- user: admin
- password: admin

Then you will be prompted to change the password. After that, you will be able
to see the dashboard, edit it and create new ones.

### Database

The database will be empty at the beginning, but you can populate it with with
dummy data provided in the `mock_data.sql` file. Before doing this, you have to
access the server through UserFeedbackConsole and create a new product, then
adding a new schema. Dummy data require the "CPU Information" template. To do
so, you can run the following command:

```bash
source .env
docker exec -i server-db-1 mysql -u root -p${MYSQL_ROOT_PASSWORD} < mock_data.sql
```

This will populate the database with some dummy data, that you can use to test
the dashboard. Be careful, since the `server-db-1` container name may change,
depending on your machine.

### Errors

The backend server is supposed to support 3 different DBMS, but actually only
one of them (sqlite) is 100% covered. This led to a set of changes to the
original project in order to fully adapt it to mysql. Even with a lot of work
and effort, there may be some incongruencies, that you are kindly invited to
report to the developers. Thank you.
