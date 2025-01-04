

export type Cols = {
    name: string
    type:string
    options: {
        primary?: boolean,
        autoincrement?: boolean,
        notnull?: boolean
        unique?: boolean
        default?: string
        check?: string
    }
}

export type Config = {
    name: string 
    value: string
    type: string
}

export type Log = {
    created: number, 
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