module Size = {
  type t =
    | Small
    | Medium
    | Large;

  let fromString = (str: String) =>
    switch str {
      | "SMALLL" => Small
      | "MEDIUM" => Medium
      | "LARGE" => Large
      | _ => Small
    };

  let toString = (size: t) =>
    switch size {
      | Small => "SMALL"
      | Medium => "MEDIUM"
      | Large => "LARGE"
    };
};

/* Define accessors for each paperclip */
module Resolve = {
  let id = (paperClip: t) => paperClip.id;
  let createdAt = (paperClip: t) => paperClip.createdAt;
  let updatedAt = (paperClip: t) => paperClip.updatedAt;
  let size = (paperClip: t) => paperClip.size |> Size.toString;
};

module Encode = {
  open Json.Encode;
  open Resolve;

  /* By opening Resolve, I can call methods from it without using the longhand: Resolve.id */
  let paperClip = (paperClip: t) =>
    object_([ ("id", paperClip |> id |> string),
    ("createdAt", paperClip |> createdAt |> JS.Date.toISOString |> string),
    ("updatedAt", paperClip |> updatedAt |> JS.Date.toISOString |> string),
    ("size", paperClip |> size |> string) ]);
};

module Decode = {
  let paperClip = (json) =>
    Json.Decode.{
      id: json |> field("id", string),
      createdAt: json |> field("created_at", string) |> JS.Date.fromString,
      updatedAt: json |> field("updated_at", string) |> JS.Date.fromString,
      size: json |> field("size", string) |> Size.fromString
    };
};

type t = {
  id: string,
  createdAt: JS.Date.t,
  updatedAt: JS.Date.t,
  size: Size.t
};

type paperClipJson = {. "id": string, "createdAt": string, "updatedAt": string, "size": string};

type paperClipInput = {. "size": string};
