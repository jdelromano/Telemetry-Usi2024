-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3307
-- Generation Time: Nov 21, 2024 at 10:54 AM
-- Server version: 8.0.40
-- PHP Version: 8.3.11

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `user_feedback`
--

-- --------------------------------------------------------

--
-- Table structure for table `pd_latest`
--

CREATE TABLE `pd_latest` (
  `col_id` int NOT NULL,
  `col_timestamp` timestamp NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Table structure for table `pd_old_version`
--

CREATE TABLE `pd_old_version` (
  `col_id` int NOT NULL,
  `col_timestamp` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `col_data_locale_language` varchar(255) DEFAULT NULL,
  `col_data_locale_region` varchar(255) DEFAULT NULL,
  `col_data_platform_os` varchar(255) DEFAULT NULL,
  `col_data_platform_version` varchar(255) DEFAULT NULL,
  `col_data_product_license` int DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Table structure for table `tbl_aggregation`
--

CREATE TABLE `tbl_aggregation` (
  `col_id` int NOT NULL,
  `col_product_id` int DEFAULT NULL,
  `col_type` varchar(255) NOT NULL,
  `col_elements` varchar(255) DEFAULT NULL,
  `col_name` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Dumping data for table `tbl_aggregation`
--

INSERT INTO `tbl_aggregation` (`col_id`, `col_product_id`, `col_type`, `col_elements`, `col_name`) VALUES
(1, 1, 'category', '[{\"schemaEntry\":\"locale\",\"schemaEntryElement\":\"language\",\"type\":\"value\"}]', 'Language Distribution'),
(2, 1, 'category', '[{\"schemaEntry\":\"locale\",\"schemaEntryElement\":\"region\",\"type\":\"value\"}]', 'Region Distribution'),
(3, 1, 'category', '[{\"schemaEntry\":\"platform\",\"schemaEntryElement\":\"os\",\"type\":\"value\"}]', 'OS Distribution'),
(4, 1, 'category', '[{\"schemaEntry\":\"platform\",\"schemaEntryElement\":\"os\",\"type\":\"value\"},{\"schemaEntry\":\"platform\",\"schemaEntryElement\":\"version\",\"type\":\"value\"}]', 'Platform Details');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_product`
--

CREATE TABLE `tbl_product` (
  `col_id` int NOT NULL,
  `col_name` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Dumping data for table `tbl_product`
--

INSERT INTO `tbl_product` (`col_id`, `col_name`) VALUES
(2, 'latest'),
(1, 'old_version');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_schema`
--

CREATE TABLE `tbl_schema` (
  `col_id` int NOT NULL,
  `col_product_id` int DEFAULT NULL,
  `col_name` varchar(255) DEFAULT NULL,
  `col_type` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Dumping data for table `tbl_schema`
--

INSERT INTO `tbl_schema` (`col_id`, `col_product_id`, `col_name`, `col_type`) VALUES
(1, 1, 'locale', 'scalar'),
(2, 1, 'platform', 'scalar'),
(3, 1, 'product', 'scalar');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_schema_element`
--

CREATE TABLE `tbl_schema_element` (
  `col_id` int NOT NULL,
  `col_schema_id` int DEFAULT NULL,
  `col_name` varchar(255) NOT NULL,
  `col_type` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Dumping data for table `tbl_schema_element`
--

INSERT INTO `tbl_schema_element` (`col_id`, `col_schema_id`, `col_name`, `col_type`) VALUES
(1, 1, 'language', 'string'),
(2, 1, 'region', 'string'),
(3, 2, 'os', 'string'),
(4, 2, 'version', 'string'),
(5, 3, 'license', 'string');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_survey`
--

CREATE TABLE `tbl_survey` (
  `col_id` int NOT NULL,
  `col_product_id` int DEFAULT NULL,
  `col_name` varchar(255) DEFAULT NULL,
  `col_url` varchar(255) NOT NULL,
  `col_active` tinyint(1) DEFAULT '0',
  `col_uuid` varchar(40) NOT NULL,
  `col_target` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Dumping data for table `tbl_survey`
--

INSERT INTO `tbl_survey` (`col_id`, `col_product_id`, `col_name`, `col_url`, `col_active`, `col_uuid`, `col_target`) VALUES
(1, 1, 'old_version_survey', 'http://localhost:1984/receiver/submit/old_version', 1, '{4fb55e79-3cc1-4ceb-9e54-90df2ab0dfc1}', 'product.version < 8.0'),
(2, 2, 'latest', 'http://localhost:1984/receiver/submit/latest', 1, '{87cf0ce0-2280-4b96-b38e-12ce4d94083d}', 'product.version >= 8.0');

-- --------------------------------------------------------

--
-- Table structure for table `tbl_version`
--

CREATE TABLE `tbl_version` (
  `col_version` int DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Dumping data for table `tbl_version`
--

INSERT INTO `tbl_version` (`col_version`) VALUES
(8);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `pd_latest`
--
ALTER TABLE `pd_latest`
  ADD PRIMARY KEY (`col_id`);

--
-- Indexes for table `pd_old_version`
--
ALTER TABLE `pd_old_version`
  ADD PRIMARY KEY (`col_id`);

--
-- Indexes for table `tbl_aggregation`
--
ALTER TABLE `tbl_aggregation`
  ADD PRIMARY KEY (`col_id`);

--
-- Indexes for table `tbl_product`
--
ALTER TABLE `tbl_product`
  ADD PRIMARY KEY (`col_id`),
  ADD UNIQUE KEY `col_name` (`col_name`);

--
-- Indexes for table `tbl_schema`
--
ALTER TABLE `tbl_schema`
  ADD PRIMARY KEY (`col_id`);

--
-- Indexes for table `tbl_schema_element`
--
ALTER TABLE `tbl_schema_element`
  ADD PRIMARY KEY (`col_id`);

--
-- Indexes for table `tbl_survey`
--
ALTER TABLE `tbl_survey`
  ADD PRIMARY KEY (`col_id`),
  ADD UNIQUE KEY `col_uuid` (`col_uuid`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `pd_latest`
--
ALTER TABLE `pd_latest`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `pd_old_version`
--
ALTER TABLE `pd_old_version`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `tbl_aggregation`
--
ALTER TABLE `tbl_aggregation`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT for table `tbl_product`
--
ALTER TABLE `tbl_product`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT for table `tbl_schema`
--
ALTER TABLE `tbl_schema`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `tbl_schema_element`
--
ALTER TABLE `tbl_schema_element`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `tbl_survey`
--
ALTER TABLE `tbl_survey`
  MODIFY `col_id` int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
