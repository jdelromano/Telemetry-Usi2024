# Use an official PHP-Apache image
FROM php:8.2-apache

# Install necessary PHP extensions for MySQL, SQLite, and PostgreSQL
RUN docker-php-ext-install pdo pdo_mysql mysqli 

# Enable Apache mod_rewrite
RUN a2enmod rewrite

# Copy the server files to the container
COPY server /var/www/html/

# Copy the Apache configuration
COPY apache-config.conf /etc/apache2/sites-available/000-default.conf

# Copy the php configuration
COPY php.ini /usr/local/etc/php/ 

# Set file permissions for Apache
RUN chown -R www-data:www-data /var/www/html

# Expose port 80 for the web server
EXPOSE 80
