# Prototype CLI + help messages by leveraging click

import click


@click.option("-v", "--version", is_flag=True, help="Print version and exit")
@click.group()
def main(version):
    """A command line OpenAI toolkit."""


@main.command(help="Run a query against an appropriate model")
@click.option("-m", "--model", help="Specify a valid chat model")
@click.option(
    "-u",
    "--no-interactive-export",
    is_flag=True,
    help="Disable [y/n] prompt that asks whether to export results",
)
@click.option("-o", "--file", help="Export results to a JSON file named FILE")
@click.option("-p", "--prompt", help="Provide prompt via command line")
@click.option(
    "-r", "--read-from-file", help="Read prompt from a custom file named FILENAME"
)
@click.option(
    "-t", "--temperature", help="Provide a sampling temperature between 0 and 2"
)
@click.option(
    "-s",
    "--store-completion",
    is_flag=True,
    help="Store results of completion on OpenAI servers",
)
def run():
    pass


@main.command(
    help="Run a query against an appropriate model but with no threading and limited verbosity"
)
@click.option("-j", "--json", is_flag=True, help="Print raw JSON response from OpenAI")
@click.option(
    "-t", "--temperature", help="Provide a sampling temperature between 0 and 2"
)
@click.option(
    "-s",
    "--store-completion",
    is_flag=True,
    help="Store results of completion on OpenAI servers",
)
def short():
    pass


@main.command(help="List available OpenAI models")
@click.option("-j", "--json", is_flag=True, help="Print raw JSON response from OpenAI")
@click.option(
    "-u",
    "--user",
    is_flag=True,
    help="List user models if they exist. Command defaults to listing OpenAI owned models",
)
def models():
    pass


@main.command(help="Get embedding representing a block of text")
@click.option("-m", "--model", help="Specify a valid embedding model")
@click.option("-i", "--input", help="Input text to embed")
@click.option("-r", "--read-from-file", help="Read input text to embed from a file")
@click.option("-o", "--output-file", help="Export embedding to FILENAME")
def embed():
    pass


# -----------------------------------------------------------------------------------------------------------
# Files
@main.group(help="Manage files uploaded to OpenAI")
def files():
    pass


@files.command(help="Get list of files uploaded to OpenAI servers")
@click.option("-j", "--json", is_flag=True, help="Print raw JSON response from OpenAI")
def list():
    pass


@files.command(help="Delete one or more uploaded files")
@click.argument("file-id", nargs=-1)
def delete(file_id):
    pass


# -----------------------------------------------------------------------------------------------------------
# Fine tuning
@main.group(help="Manage fine tuning operations")
def fine_tune():
    pass


@fine_tune.command(help="Upload a fine-tuning file")
@click.argument("file")
def upload_file():
    pass


@fine_tune.command(help="Create a fine-tuning job")
@click.argument("file-id")
@click.argument("model")
def create_job(file_id, model):
    pass


@fine_tune.command(help="Delete a fine-tuned model")
@click.argument("model-id")
def delete_model(model_id):
    pass


@fine_tune.command(help="List fine-tuning jobs")
@click.option("-j", "--json", is_flag=True, help="Print raw JSON response from OpenAI")
@click.option("-l", "--limit", help="Show LIMIT number of fine-tuning jobs")
def list_jobs():
    pass


# -----------------------------------------------------------------------------------------------------------
@main.command(help="Get OpenAI usage details")
@click.option("-j", "--json", is_flag=True, help="Print raw JSON response from OpenAI")
@click.option("-d", "--days", help="Select number of days to go back")
def costs():
    pass


@main.command(help="Edit one or more files according to a prompt [DEPRECATED]")
def edit():
    pass


@main.command(help="Generate an image from a prompt")
@click.option(
    "-q",
    "--hd",
    is_flag=True,
    help="Request high definition image (default is standard definition for cost savings)",
)
@click.option(
    "-v",
    "--vivid",
    is_flag=True,
    help="Request hyper-realistic / dramatic image (default is natural)",
)
def img():
    pass


if __name__ == "__main__":
    main()
