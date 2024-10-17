# Use the official PHP image as the base image
FROM php:8.0-apache
COPY ./php.ini /usr/local/etc/php/conf.d/php.ini
# Install necessary PHP extensions for MySQL
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Set the working directory inside the container
WORKDIR /var/www/html

# Copy the PHP application code into the container
COPY ./server/ .

# Expose the port the PHP built-in server will run on
EXPOSE 80

