#include <stdio.h>
#include <stdlib.h>
#include <orca/discord.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char *forbidden_words_filename;
char *forbidden_words;

void read_words()
{
	FILE *in = fopen(forbidden_words_filename, "r");
	if (in == NULL)
	{
		printf("File not found!\n");
		exit(1);
	}
	struct stat sb;
	if (stat(forbidden_words_filename, &sb) == -1)
	{
		printf("Error while trying to find size of file\n");
		exit(1);
	}
	size_t size = sb.st_size;
	char *buf = malloc(size + 1);
	fread(buf, sizeof(char), size, in);
	int i = 0;
	forbidden_words = buf;
	i = 0;
	while (i != size) {
		if (buf[i] == 10) buf[i] = 0;
		i++;
	}
}

int in_list(char *msg_content)
{
	char *message_content = msg_content;
	char *buf = forbidden_words;
	while (strlen(buf) > 0)
	{
		if (strstr(message_content, buf)) return 1;
		message_content = msg_content;
		buf += strlen(buf);
		buf++;
	}
	return 0;
}

void on_ready(const struct discord* client)
{
	const struct discord_user *bot = discord_get_self(client);
	log_info("Logged in as %s!", bot->username);
	log_info("Now reading all words in designated file (%s)", forbidden_words_filename);
	read_words();
	log_info("Done reading file");
}

void on_message(const struct dicord *client, const struct discord_message *msg)
{
	if (in_list(msg->content))
	{
		printf("This message contains a forbidden word\n");
		discord_async_next(client, NULL);
		struct discord_create_message_params response = {.content = "This message contains a forbidden word!"};
		discord_create_message(client, msg->channel_id, &response, NULL);
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("[ERROR]: Expected 1 argument but got : [%d]\n", argc - 1);
		exit(1);
	}
	forbidden_words_filename = 1[argv];
	struct discord *client = discord_config_init("config.json");
	discord_set_on_ready(client, on_ready);
	discord_set_on_message_create(client, on_message);
	discord_run(client);
	discord_cleanup(client);
	return 0;
}
