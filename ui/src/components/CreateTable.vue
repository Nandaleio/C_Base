<script setup lang="ts">
import {
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle
} from '@/components/ui/dialog'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select'
import { Trash2 } from 'lucide-vue-next'
import { ref } from 'vue';

const tableName = ref<string>('');
const column = ref<{name:string, type:string}[]>([]);
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
            <Input type="text" name="tableName" placeholder="Table Name"  v-model="col.name"/>
            <Select v-model="col.type">
                <SelectTrigger>
                <SelectValue placeholder="Column type" />
                </SelectTrigger>
                <SelectContent>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                </SelectContent>
            </Select>
            <Button variant="destructive" @click="column.splice(column.indexOf(col), 1)">
                <Trash2 />
            </Button>
        </div>

        <Button variant="outline" @click="column.push({name:'', type:''})">Add Column</Button>

        <DialogFooter>
            <Button @click="">
                Create Table
            </Button>
        </DialogFooter>
    </DialogContent>
</template>