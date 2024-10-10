# Use the official PHP image as the base image
FROM php:8.2-cli
COPY ./php.ini /usr/local/etc/php/php.ini
# Install necessary PHP extensions for MySQL
RUN docker-php-ext-install mysqli pdo pdo_mysql

# Set the working directory inside the container
WORKDIR /app

# Copy the PHP application code into the container
COPY . /app

WORKDIR /app/testserver

# Expose the port the PHP built-in server will run on
EXPOSE 1984

# Command to run the PHP built-in server
CMD ["php", "-S", "0.0.0.0:1984", "router.php"]
