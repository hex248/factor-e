# Item
items.json

| property name    | type          | note                                                         |
| ---------------- | ------------- | ------------------------------------------------------------ |
| id               | unsigned char |                                                              |
| name             | std::string   |                                                              |
| iconSpritePath   | std::string   | path to icon sprite to be used in UI                         |
| inHandSpritePath | std::string   | path to sprite to use when "in hand"                         |
| iconScale        | float         |                                                              |
| inHandScale      | float         |                                                              |
| tileID           | unsigned char | 0 indicates no tile (?) - references tile in tile_types.json |
| stackSize        | unsigned int  | maximum of the item that can occupy 1 slot                   |
| placeable        | bool          |                                                              |
| showInHand       | bool          |                                                              |
# ItemStack

| property name | type          | note                               |
| ------------- | ------------- | ---------------------------------- |
| id            | unsigned char |                                    |
| itemID        | unsigned char | references item in items.json      |
| quantity      | unsigned int  | amount of these items in the stack |
| iconSprite    | Texture2D     |                                    |
| inHandSprite  | Texture2D     |                                    |
# Player (stored in player.bin)

| property name | type          | note                                      |
| ------------- | ------------- | ----------------------------------------- |
| id            | unsigned char |                                           |
| inventory     | ItemStack[21] | 3 rows of 7 (toolbelt bar is indexes 0-6) |
| sprite        | Texture2D     | (not saved)                               |
| position      | Vector2       |                                           |
|               |               |                                           |
|               |               |                                           |
|               |               |                                           |
|               |               |                                           |
|               |               |                                           |
