

export type Cols = {
    name: string
    type:string
}

export const DataType = {
    1: 'INTEGER',
    2: 'FLOAT',
    3: 'TEXT',
    4: 'BLOB',
    5: 'NULL',

} as const;