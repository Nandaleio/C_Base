<script setup lang="ts">
import {
    DialogContent,
    DialogDescription,
    DialogFooter,
    DialogHeader,
    DialogTitle,
    DialogClose,
    Dialog,
    DialogTrigger,
} from '@/components/ui/dialog'

import {
    Tooltip,
    TooltipContent,
    TooltipProvider,
    TooltipTrigger,
} from '@/components/ui/tooltip'
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import { Plus } from 'lucide-vue-next'

import ColumnDefinition from '@/components/ColumnDefinition.vue';
import { ref } from 'vue';
import type { Cols } from '@/utils/types';
import { cbFetch } from '@/services/api-service';
import { useToast } from './ui/toast';

const tableName = ref<string>('');
const column = ref<Cols[]>([
    { name: 'id', type: 'INTEGER', options: { primary: true, autoincrement: true } },
    { name: 'created', type: 'INTEGER', options: { default: "(unixepoch('now'))" } },
]);

const open = ref(false)

const { toast } = useToast();

async function createTable() {
    if (!tableName.value) {
        toast({
            title: 'The table need a name',
            description: 'Please enter a name for the table',
            variant: 'destructive',
        });
        return;
    }
    const res = await cbFetch(`/api/admin/table/${tableName.value}`, 'POST', column.value);
    if (!res.error) {
        emits('created');
        open.value = false;
    };
}

const emits = defineEmits<{
    created: [],
    cancel: []
}>();
</script>

<template>

    <Dialog :open="open" @update:open="(o) => open = o">
        <DialogTrigger>
            <TooltipProvider>
                <Tooltip>
                    <TooltipTrigger as-child>
                        <Plus />
                    </TooltipTrigger>
                    <TooltipContent>
                        <p>Create a new table</p>
                    </TooltipContent>
                </Tooltip>
            </TooltipProvider>
        </DialogTrigger>
        <DialogContent>
            <DialogHeader>
                <DialogTitle>Create a new table</DialogTitle>
                <DialogDescription>
                    Add columns
                </DialogDescription>
            </DialogHeader>

            <Input type="text" name="tableName" placeholder="Table Name" v-model="tableName" />

            <div class="flex items-center" v-for="col of column">
                <ColumnDefinition :col="col" @delete="column.splice(column.indexOf(col), 1)" />
            </div>

            <Button variant="outline" @click="column.push({ name: '', type: '', options: {} })">Add Column</Button>

            <DialogFooter>
                <DialogClose as-child>
                    <Button type="button" variant="secondary">
                        Cancel
                    </Button>
                </DialogClose>
                <Button @click="createTable">
                    Create Table
                </Button>
            </DialogFooter>
        </DialogContent>
    </Dialog>
</template>