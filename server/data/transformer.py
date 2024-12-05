import sys, re


def str_to_sql(line: str) -> str:
    values = line.split("very_cool_unique_separator")
    return "INSERT INTO user_feedback.pd_banana_logs (col_id, col_timestamp, col_data_parameters_app, col_data_parameters_version_number, col_data_parameters_region, col_data_parameters_language, col_data_parameters_operating_system) VALUES (NULL, CURRENT_TIMESTAMP, '{}', '{}', '{}', '{}', '{}');\n".format(
        values[1], values[2], values[3], values[4], values[5]
    )


def transform(file: str) -> set:
    result = set()
    with open(file, "r") as f:
        for line in f:
            # Match IP and URLs starting with /portal
            match = re.match(
                r'(?P<ip>\d+\.\d+\.\d+\.\d+) - - \[.*\] "GET (?P<url>/portal[^\s]*) HTTP/.*"',
                line,
            )
            if not match:
                continue

            ip = match.group("ip")
            url = match.group("url")

            # Extract parameters from the URL
            params = re.findall(r"(\w+)=([^&]+)", url)
            param_dict = dict(params)

            # Check if all required fields are present
            required_keys = ["app", "vno", "cty", "lng", "osd"]
            if set(required_keys).issubset(param_dict.keys()):
                list_of_values = [param_dict[key] for key in required_keys]
                list_of_values = [ip] + list_of_values
                result.add(
                    "very_cool_unique_separator".join(list_of_values)
                    .replace("%20", " ")
                    .replace("%2F", "/")
                    .replace("%2C", ",")
                    .replace("%3A", ":")
                    .replace("%2D", " ")
                    .replace("%25", "")
                    .replace("-", " ")
                )
    return result


def main() -> None:
    # Ensure the usage is correct
    if "-o" not in sys.argv or len(sys.argv) < 4:
        print("Usage: python script.py <input1> <input2> ... -o <output>")
        sys.exit(1)

    # Locate the -o flag
    try:
        o_index = sys.argv.index("-o")
    except ValueError:
        print("Error: Missing '-o' flag.")
        sys.exit(1)

    # Get input files and output file
    input_files = sys.argv[1:o_index]
    output_file = sys.argv[o_index + 1]

    if not input_files or not output_file:
        print("Error: No input files or output file specified.")
        sys.exit(1)

    try:
        result = set()
        for file in input_files:
            result |= transform(file)
        # print(result)
        with open(output_file, "w") as f:
            f.write("truncate table user_feedback.pd_banana_logs;\n")
            for line in result:
                f.write(str_to_sql(line))
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
