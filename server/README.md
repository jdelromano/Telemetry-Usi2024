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

- The Apache server is a lightweight and basic image, to keep everything as slim
  and simple as possible.
- The database is mysql and not sqlite due to requirements given by Banana.ch.
- Grafana is a powerful tool to visualize data, and it is used to display data
  stored in the database.

### First steps

Everything will deploy automatically, you just have to login to grafana with the
following credentials:

- user: admin
- password: admin

Then you will be prompted to change the password. After that, you will be able
to see the dashboard, edit it and create new ones.

### Database

The database will be empty at the beginning, but you can populate it with with
dummy data provided in the `mock_data.sql` file. To do so, you can run the
following command:

```bash
source .env
docker exec -i kuserfeedback-db mysql -u root -p${MYSQL_ROOT_PASSWORD} < mock_data.sql
```

This will populate the database with some dummy data, that you can use to test
the dashboard.

### Future improvements & Customization

You can add/remove fields by changing the relative schema in the
UserFeedbackConsole GUI application. This will also require some changes to the
Grafana dashboard, since data won't be displayed automatically. Removing data
from the schema will result in a malfunction of the dashboard, but **only the
panels using that data will be affected**, since the system is designed to be as
flexible and containerized as possible. Grafana supports both SQL queries and
graphical queries, so it allows inexperienced users to create their own
dashboards.

### Errors & Maintenance

The backend server is supposed to support 3 different DBMS, but actually only
one of them (sqlite) is 100% covered. This led to a set of changes to the
original project in order to fully adapt it to mysql. Even with a lot of work
and effort, there may be some incongruencies, that you are kindly invited to
report to the developers. Thank you.
