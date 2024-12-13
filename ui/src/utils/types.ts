

export type Cols = {
    name: string
    type:string
}

export type Config = {
    name: string 
    value: string
    type: string
}

export type Log = {
    created: Date, 
    level: string, 
    description: string
}

export const DataType = {
    1: 'INTEGER',
    2: 'FLOAT',
    3: 'TEXT',
    4: 'BLOB',
    5: 'NULL',

} as const;