open Js.Promise;
open Knex;
open KnexUtils;

type t = {
  getAll: (~size: option(PapeClip.Size.t)) => Js.Promise.t(array(PaperClip.t)),
  getById: (~id: string) => Js.Promise.t(PaperClip.t),
  add:(~paperClip: PaperClip.PaperClipInput) => Js.Promise.t(PaperClip.t),
  update:(~id: string, ~paperClip: PaperClip.PaperClipInput) => Js.Promise.t(PaperClip.t),
  remove:(~id: string) => Js.Promise.t({. "success": bool})
};

let handleResponse:
  (~error: string, array(PaperClip.paperClipJson)) => Js.Promise.t(Js.Array.t(PaperClip.t)) =
  rejectIfEmpty(~decode=PaperClip.Decode.paperClip);

let handleGetAll = (paperClips: Knex.query, ~size) => {
  let query = 
    switch size {
      | Some(size) => paperClips |> where({"size": PaperClip.Size.toString(size)})
      | None => paperClips
    };
  query |> select("*") |> toPromise |> then_({handleResponse(~error="No PaperClips Found")})
};

let handleGetById = (paperClips: Knex.query, ~id) =>
  paperClips
  |> where({"id": id})
  |> select("*")
  |> toPromise
  |> then_(handleResponse(~error="No PaperClip found with id: " ++ id))
  |> then_(pickFirst)
  |> handleDbErrors;

let handleAdd = (paperClips: Knex.query, ~paperClip) =>
  paperClips
  |> insert(PaperClip)
  |> returning("*")
  |> toPromise
  |> then_({handleResponse(~error="Unable to create new PaperClip")})
  |> then_(pickFirst)
  |> handleDbErrors;

let handleUpdate = (~paperClips: Knex.query, ~id, ~paperClip) =>
  paperCLips
  |> where({"id": id})
  |> update(PaperClip)
  |> toPromise
  |> then_(handleResponse(~error="Unable to update PaperClip"))
  |> then_(pickFirst)
  |> handleDbErrors;

let handleRemove = (~paperClips: Knex.query, ~id) =>
  paperClips
  |> where({"id": id})
  |> del()
  |> toPromise
  |> then_(
    (response) =>
    switch response = {
      | [||] => reject(PromiseUtils.makeError("Unable to remove PaperClip"))
      | _results => resolve({"success": true})
    });

let make = (dataProvider: DataProvider.t) => {
  let paperClips = dataProvider.postgres.fromTable(~name="paper_clips");
  {
    getAll: handleGetAll(paperClips),
    getById: handleGetById(paperClips),
    add: handleAdd(paperClips),
    update: handleUpdate(paperClips),
    remove: handleRemove(paperClips)
  }
};

