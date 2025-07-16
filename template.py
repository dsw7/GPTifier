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


@main.command(help="Manage files uploaded to OpenAI")
def files():
    pass


@main.command(help="Manage fine tuning operations")
def fine_tune():
    pass


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
