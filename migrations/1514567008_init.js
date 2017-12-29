exports.up = function (knex, Promise) {
  return (
    Promise.resolve()

    .then(() =>
      knex.schema.createTable('paper_clips', function (table) {
        table.uuid('id')
          .primary()
          .notNullable()
          .unique()
          .defaultTo(knex.raw('uuid_generate_v4()'))
        table.string('size').notNullable()
        table.timestamps(true, true)
      })
    )
  )
}

exports.down = function (knex, Promise) {
  throw new Error(
    'No Down Migrations Supported'
  )
}
