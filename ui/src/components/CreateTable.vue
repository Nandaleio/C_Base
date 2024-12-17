<script setup lang="ts">
import {
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogClose
} from '@/components/ui/dialog'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';

import ColumnDefinition from '@/components/ColumnDefinition.vue';
import { ref } from 'vue';
import type { Cols } from '@/utils/types';

const tableName = ref<string>('');
const column = ref<Cols[]>([
    {name: 'id', type: 'INTEGER', options: { primary: true, autoincrement: true}},
    {name: 'created', type: 'INTEGER', options: { default: '(CURRENT_TIMESTAMP)'}},
]);
</script>

<template>

    <DialogContent>
        <DialogHeader>
            <DialogTitle>Create a new table</DialogTitle>
            <DialogDescription>
                Add columns
            </DialogDescription>
        </DialogHeader>

        <Input type="text" name="tableName" placeholder="Table Name" v-model="tableName"/>

        <div class="flex items-center" v-for="col of column">
            <ColumnDefinition :col="col" @delete="column.splice(column.indexOf(col), 1)" />
        </div>

        <Button variant="outline" @click="column.push({name:'', type:'', options: {}})">Add Column</Button>

        <DialogFooter>
            <DialogClose as-child>
                    <Button type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
            <Button @click="console.log(column)">
                Create Table
            </Button>
        </DialogFooter>
    </DialogContent>
</template>