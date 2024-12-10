#!/usr/bin/env python3
#this is the main file to run for generating a library testing harness with an LLM



#for command line parameters
import click

#for llm
from langchain_openai import ChatOpenAI


#loaders
from langchain_community.document_loaders import DirectoryLoader, TextLoader, WebBaseLoader
from langchain_community.document_loaders.merge import MergedDataLoader


#for vector store
#from langchain_community.embeddings import OpenAIEmbeddings
from langchain_openai import OpenAIEmbeddings
from langchain_community.vectorstores import FAISS
from langchain.text_splitter import RecursiveCharacterTextSplitter


#for agent
from langchain_core.prompts import ChatPromptTemplate
from langchain_core.output_parsers import StrOutputParser
from langchain.chains.question_answering import load_qa_chain


#global parameters / variables

#this should be done with env variables!! please no steal
api_key = "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"

divider = "SEED ADVICE"
quote_chars = "```"

system_prompt = f""" You will be given a C library name and should make a harness to fuzz it using AFL++. You will be given some context in the form of scraped documentation. First make a harness program named 'harness.c' that has a main method that recieves command line input args and runs the harness. Then create a makefile that builds the harness and has an 'afl' option which runs the correct afl command to fuzz and links to a locally compiled versoin of the library. Finally provided the commands or resources needed to generate seed files (or the text for a seed file) and put {divider} before this section. """

#Seperate these sections with a '{divider}' around each section's code (before and after each section).


#can be changed for other LLMs
llm = ChatOpenAI(
    model_name="gpt-4o", #also try gpt-3.5-turbo
    api_key=api_key
    )

#the embeddings to use for vector searching
embeddings = OpenAIEmbeddings(api_key=api_key)

#helper functions
def save_to_file(name, data, out_file_location):
    if out_file_location[-1] != "/":
        output = out_file_location + "/" + name
    else:
        output = out_file_location + name

    with open(output, "w") as out_file:
        out_file.write(data)
    print(f"Wrote {output}")

supported_file_types = {
    "c":"harness.c",
    "makefile":"Makefile",
    "make":"Makefile",
    "bash":"seeds.sh",
    "sh":"seeds.sh",
    }

def break_into_sections(text, out_file_location):
    lines = text.split('\n')
    lines_iter = iter(lines)
    for l in lines_iter:
        if quote_chars in l:
            file_type = l.replace(quote_chars,"")
            file = ""
            for cl in lines_iter:
                if quote_chars in cl:
                    break
                file += (f"\n{cl}")
                #print(cl)
            #print(file)
            if file_type in supported_file_types:
                print(f"found a {file_type} file!")
                file_name = supported_file_types[file_type]
                save_to_file(file_name, file, out_file_location)
            else:
                print(f"Error, {file_type} is not a supported file type!")


def get_document(text_dir, urls):
    #text files
    text_loader = DirectoryLoader(text_dir, glob='**/*.txt', loader_cls=TextLoader)

    #urls
    url_loader = WebBaseLoader(urls)


    if text_dir == 'None' and len(urls) > 0:
        print('Just loading urls')
        docs_all = url_loader.load()
    elif text_dir != 'None' and len(urls) == 0:
        print('Just loading text files')
        docs_all = text_loader.load()
    elif text_dir != 'None' and len(urls) > 0:
        #merge
        print('Loading both text files and urls')
        loader_all = MergedDataLoader(loaders=[url_loader, text_loader])
        docs_all = loader_all.load()
    else:
        docs_all = None
        return docs_all


    text_splitter = RecursiveCharacterTextSplitter(chunk_size=500, chunk_overlap=100)  # Adjust size as needed
    chunks = text_splitter.split_documents(docs_all)

    # for debug
    #print("Testing raw documents")
    #print(docs_all)
    #print("Testing chunks")
    #print(chunks)

    return chunks


@click.command()
@click.option('--library_name', '-n', prompt='Library Name to Harness', help='The library to fuzz')
@click.option('--url_file_name', '-u', default='None', help='Text file of newline seperated urls to scrape for documentation info')
@click.option('--target', '-t', default='the most important functions', help='Provide additional context to make a more targeted harness (will vector search docs)')
@click.option('--number_of_contexts', '-q', default=3, help='Number of queries for the vector database')
@click.option('--out_file_location', '-o', default='./', help='Where to write the output files')
@click.option('--text_files_dir', '-txt', default='None', help='The directoy you would like to load text file context from')
def main(library_name, url_file_name, target, number_of_contexts, out_file_location, text_files_dir):
    urls = []

    try:
        with open(url_file_name, "r") as url_file:
            for line in url_file:
                urls.append(line.strip())
    except:
        print("urls file not found!")

    print(urls)
    docs_all = get_document(text_files_dir, urls)

    #vector store


    if docs_all != None:
        vector_store = FAISS.from_documents(docs_all, embeddings)
        docs = vector_store.similarity_search(target, k=number_of_contexts)
        docs = "\n\n---\n\n".join(doc.page_content for doc in docs)
    else:
        docs = "No documents to show"
    #print("your docs are:")
    #print(docs)

    prompt = ChatPromptTemplate.from_messages([
        ("system", system_prompt),
        ("user", "Please fuzz: '{library_name}'"),
        ("user", "with the specific target of: '{target}'"),
        ("user", "Here are some documents related to your question:\n\n{docs}"),
    ])
    #print(f"this is the prompt: \n{prompt}")

    output_parser = StrOutputParser()
    output = prompt | llm | output_parser

    response = output.invoke({"library_name": library_name, "target": target, "docs": docs})
    #print(response)

    break_into_sections(response, out_file_location)
    try:
        print("Here is some useful advice for generating seeds")
        seeds = response.split(divider)[1]
        print(seeds)
    except:
        print("No additional seed information")





if __name__ == "__main__":
    main()

