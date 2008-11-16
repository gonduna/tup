#include "memdb.h"
#include "array_size.h"
#include <stdio.h>

int memdb_init(struct memdb *m)
{
	int x;
	int rc;
	const char *sql[] = {
		"create table node_map (id integer primary key not null, ptr integer not null)",
	};

	rc = sqlite3_open(":memory:", &m->db);
	if(rc != 0) {
		fprintf(stderr, "Unable to create in-memory database: %s\n",
			sqlite3_errmsg(m->db));
		return -1;
	}
	for(x=0; x<ARRAY_SIZE(sql); x++) {
		char *errmsg;
		if(sqlite3_exec(m->db, sql[x], NULL, NULL, &errmsg) != 0) {
			fprintf(stderr, "SQL error: %s\nQuery was: %s",
				errmsg, sql[x]);
			return -1;
		}
	}
	return 0;
}

int memdb_add(struct memdb *m, tupid_t id, void *n)
{
	int rc;
	static sqlite3_stmt *stmt = NULL;
	static char s[] = "insert into node_map(id, ptr) values(?, ?)";

	if(!stmt) {
		if(sqlite3_prepare_v2(m->db, s, sizeof(s), &stmt, NULL) != 0) {
			fprintf(stderr, "SQL Error: %s\nStatement was: %s\n",
				sqlite3_errmsg(m->db), s);
			return -1;
		}
	}

	if(sqlite3_bind_int64(stmt, 1, id) != 0) {
		fprintf(stderr, "SQL bind error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}
	if(sqlite3_bind_int(stmt, 2, (int)n) != 0) {
		fprintf(stderr, "SQL bind error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}

	rc = sqlite3_step(stmt);
	if(sqlite3_reset(stmt) != 0) {
		fprintf(stderr, "SQL reset error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}

	if(rc != SQLITE_DONE) {
		fprintf(stderr, "SQL step error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}

	return 0;
}

int memdb_remove(struct memdb *m, tupid_t id)
{
	int rc;
	static sqlite3_stmt *stmt = NULL;
	static char s[] = "delete from node_map where id=?";

	if(!stmt) {
		if(sqlite3_prepare_v2(m->db, s, sizeof(s), &stmt, NULL) != 0) {
			fprintf(stderr, "SQL Error: %s\nStatement was: %s\n",
				sqlite3_errmsg(m->db), s);
			return -1;
		}
	}

	if(sqlite3_bind_int64(stmt, 1, id) != 0) {
		fprintf(stderr, "SQL bind error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}

	rc = sqlite3_step(stmt);
	if(sqlite3_reset(stmt) != 0) {
		fprintf(stderr, "SQL reset error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}

	if(rc != SQLITE_DONE) {
		fprintf(stderr, "SQL step error: %s\n", sqlite3_errmsg(m->db));
		return -1;
	}

	return 0;
}

void *memdb_find(const struct memdb *m, tupid_t id)
{
	int dbrc;
	void *ptr;
	static sqlite3_stmt *stmt = NULL;
	static char s[] = "select ptr from node_map where id=?";
	int res;

	if(!stmt) {
		if(sqlite3_prepare_v2(m->db, s, sizeof(s), &stmt, NULL) != 0) {
			fprintf(stderr, "SQL Error: %s\nStatement was: %s\n",
				sqlite3_errmsg(m->db), s);
			return NULL;
		}
	}

	if(sqlite3_bind_int64(stmt, 1, id) != 0) {
		fprintf(stderr, "SQL bind error: %s\n", sqlite3_errmsg(m->db));
		return NULL;
	}

	dbrc = sqlite3_step(stmt);
	if(dbrc == SQLITE_DONE) {
		ptr = NULL;
		goto out_reset;
	}
	if(dbrc != SQLITE_ROW) {
		fprintf(stderr, "SQL step error: %s\n", sqlite3_errmsg(m->db));
		ptr = NULL;
		goto out_reset;
	}

	res = sqlite3_column_int(stmt, 0);
	ptr = (struct node*)res;

out_reset:
	if(sqlite3_reset(stmt) != 0) {
		fprintf(stderr, "SQL reset error: %s\n", sqlite3_errmsg(m->db));
		return NULL;
	}

	return ptr;
}
